//===- A64ExpandPseudoInsts.cpp - Expand pseudo instructions --------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains a pass that expands pseudo instructions into target
// instructions to allow proper scheduling and other late optimizations.  This
// pass should be run after register allocation but before the post-regalloc
// scheduling pass.
//
//===----------------------------------------------------------------------===//

#include "A64InstrInfo.h"
#include "MCTargetDesc/A64AddressingModes.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/Pass.h"

using namespace llvm;

#define A64_EXPAND_PSEUDO_NAME "A64 pseudo instruction expansion pass"

namespace {

class A64ExpandPseudo : public MachineFunctionPass {
public:
  const A64InstrInfo *TII;

  static char ID;

  A64ExpandPseudo() : MachineFunctionPass(ID) {
    initializeA64ExpandPseudoPass(*PassRegistry::getPassRegistry());
  }

  bool runOnMachineFunction(MachineFunction &Fn) override;

  StringRef getPassName() const override { return A64_EXPAND_PSEUDO_NAME; }

private:
  bool expandMBB(MachineBasicBlock &MBB);
  bool expandMI(MachineBasicBlock &MBB, MachineBasicBlock::iterator MBBI,
                MachineBasicBlock::iterator &NextMBBI);

  bool expandMOVImm(MachineBasicBlock &MBB, MachineBasicBlock::iterator MBBI);
};
} // end anonymous namespace

char A64ExpandPseudo::ID = 0;

INITIALIZE_PASS(A64ExpandPseudo, "a64-expand-pseudo", A64_EXPAND_PSEUDO_NAME,
                false, false)

/// Transfer implicit operands on the pseudo instruction to the
/// instructions created from the expansion.
static void transferImpOps(MachineInstr &OldMI, MachineInstrBuilder &UseMI,
                           MachineInstrBuilder &DefMI) {
  const MCInstrDesc &Desc = OldMI.getDesc();
  for (unsigned i = Desc.getNumOperands(), e = OldMI.getNumOperands(); i != e;
       ++i) {
    const MachineOperand &MO = OldMI.getOperand(i);
    assert(MO.isReg() && MO.getReg());
    if (MO.isUse())
      UseMI.add(MO);
    else
      DefMI.add(MO);
  }
}

struct ImmInsnModel {
  unsigned Opcode;
  uint64_t Op1;
  uint64_t Op2;
};

/// \brief Expand a MOVi64imm pseudo instruction to a
/// MOVZ or MOVN followed by up to 3 MOVK instructions.
static inline void expandMOVImmSimple(uint64_t Imm,
                                      SmallVectorImpl<ImmInsnModel> &Insn) {
  const unsigned Mask = 0xFFFF;

  // Use a MOVZ or MOVN instruction to set the high bits, followed by one or
  // more MOVK instructions to insert additional 16-bit portions into the
  // lower bits.
  bool isNeg = false;

  // Scan the immediate and count the number of 16-bit chunks which are either
  // all ones or all zeros.
  unsigned OneChunks = 0;
  unsigned ZeroChunks = 0;
  for (unsigned Shift = 0; Shift < 64; Shift += 16) {
    const unsigned Chunk = (Imm >> Shift) & Mask;
    if (Chunk == Mask)
      OneChunks++;
    else if (Chunk == 0)
      ZeroChunks++;
  }

  // Use MOVN to materialize the high bits if we have more all one chunks
  // than all zero chunks.
  if (OneChunks > ZeroChunks) {
    isNeg = true;
    Imm = ~Imm;
  }

  unsigned FirstOpc = (isNeg ? A64::MOVN : A64::MOVZ);

  unsigned Shift = 0;     // LSL amount for high bits with MOVZ/MOVN
  unsigned LastShift = 0; // LSL amount for last MOVK
  if (Imm != 0) {
    unsigned LZ = countLeadingZeros(Imm);
    unsigned TZ = countTrailingZeros(Imm);
    Shift = (TZ / 16) * 16;
    LastShift = ((63 - LZ) / 16) * 16;
  }
  unsigned Imm16 = (Imm >> Shift) & Mask;
  /// Can we add instruction directly here
  Insn.push_back({FirstOpc, Imm16, A64_AM::getShifterImm(A64_AM::LSL, Shift)});

  if (Shift == LastShift)
    return;

  // If a MOVN was used for the high bits of a negative value, flip the rest
  // of the bits back for use with MOVK.
  if (isNeg)
    Imm = ~Imm;

  unsigned Opc = A64::MOVK;
  while (Shift < LastShift) {
    Shift += 16;
    Imm16 = (Imm >> Shift) & Mask;
    if (Imm16 == (isNeg ? Mask : 0))
      continue; // This 16-bit portion is already set correctly.

    Insn.push_back({Opc, Imm16, A64_AM::getShifterImm(A64_AM::LSL, Shift)});
  }
}

bool A64ExpandPseudo::expandMOVImm(MachineBasicBlock &MBB,
                                   MachineBasicBlock::iterator MBBI) {
  MachineInstr &MI = *MBBI;
  Register DstReg = MI.getOperand(0).getReg();
  uint64_t RenamableState =
      MI.getOperand(0).isRenamable() ? RegState::Renamable : 0;
  uint64_t Imm = MI.getOperand(1).getImm();

  if (DstReg == A64::XZR) {
    // Useless def, and we don't want to risk creating an invalid ORR (which
    // would really write to sp).
    MI.eraseFromParent();
    return true;
  }

  SmallVector<ImmInsnModel, 4> Insn;

  expandMOVImmSimple(Imm, Insn);
  assert(Insn.size() != 0);

  SmallVector<MachineInstrBuilder, 4> MIBS;
  for (auto I = Insn.begin(), E = Insn.end(); I != E; ++I) {
    bool LastItem = std::next(I) == E;
    switch (I->Opcode) {
    default:
      llvm_unreachable("unhandled!");
      break;
    case A64::MOVN:
    case A64::MOVZ: {
      bool DstIsDead = MI.getOperand(0).isDead();
      MIBS.push_back(
          BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(I->Opcode))
              .addReg(DstReg, RegState::Define |
                                  getDeadRegState(DstIsDead && LastItem) |
                                  RenamableState)
              .addImm(I->Op1)
              .addImm(I->Op2));
    } break;
    case A64::MOVK: {
      Register DstReg = MI.getOperand(0).getReg();
      bool DstIsDead = MI.getOperand(0).isDead();
      MIBS.push_back(
          BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(I->Opcode))
              .addReg(DstReg, RegState::Define |
                                  getDeadRegState(DstIsDead && LastItem) |
                                  RenamableState)
              .addReg(DstReg)
              .addImm(I->Op1)
              .addImm(I->Op2));
    } break;
    }
  }
  transferImpOps(MI, MIBS.front(), MIBS.back());
  MI.eraseFromParent();
  return true;
}

/// If MBBI references a pseudo instruction that should be expanded here,
/// do the expansion and return true.  Otherwise return false.
bool A64ExpandPseudo::expandMI(MachineBasicBlock &MBB,
                               MachineBasicBlock::iterator MBBI,
                               MachineBasicBlock::iterator &NextMBBI) {
  MachineInstr &MI = *MBBI;
  unsigned Opcode = MI.getOpcode();

  switch (Opcode) {
  default:
    break;
  // Xrr for register, register without a Shift, translate to
  // A left shift of 0.
  case A64::ADDXrr:
  case A64::SUBXrr:
  case A64::ADDSXrr:
  case A64::SUBSXrr: {
    if (Opcode == A64::ADDXrr)
      Opcode = A64::ADDXrs;
    else if (Opcode == A64::SUBXrr)
      Opcode = A64::SUBXrs;
    else if (Opcode == A64::ADDSXrr)
      Opcode = A64::ADDSXrs;
    else
      Opcode = A64::SUBSXrs;
    MachineInstrBuilder MIB1 =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(Opcode),
                MI.getOperand(0).getReg())
            .add(MI.getOperand(1))
            .add(MI.getOperand(2))
            .addImm(A64_AM::getShifterImm(A64_AM::LSL, 0));
    transferImpOps(MI, MIB1, MIB1);
    MI.eraseFromParent();
    return true;
    break;
  }
  case A64::MOVaddr: {
    // MOVaddr translates to an ADRP, ADD pair.
    Register DstReg = MI.getOperand(0).getReg();
    MachineInstrBuilder MIB1 =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(A64::ADRP), DstReg)
            .add(MI.getOperand(1));
    MachineInstrBuilder MIB2 =
        BuildMI(MBB, MBBI, MI.getDebugLoc(), TII->get(A64::ADDiXri))
            .add(MI.getOperand(0))
            .addReg(DstReg)
            .add(MI.getOperand(2))
            .addImm(0);

    transferImpOps(MI, MIB1, MIB2);
    MI.eraseFromParent();
    return true;
  }
  case A64::MOVi64imm:
    // Expands into several MOV instructions.
    return expandMOVImm(MBB, MBBI);
  }
  return false;
}

/// Iterate over the instructions in basic block MBB and expand any
/// pseudo instructions.  Return true if anything was modified.
bool A64ExpandPseudo::expandMBB(MachineBasicBlock &MBB) {
  bool Modified = false;

  MachineBasicBlock::iterator MBBI = MBB.begin(), E = MBB.end();
  while (MBBI != E) {
    MachineBasicBlock::iterator NMBBI = std::next(MBBI);
    Modified |= expandMI(MBB, MBBI, NMBBI);
    MBBI = NMBBI;
  }

  return Modified;
}

bool A64ExpandPseudo::runOnMachineFunction(MachineFunction &MF) {
  TII = static_cast<const A64InstrInfo *>(MF.getSubtarget().getInstrInfo());

  bool Modified = false;
  for (auto &MBB : MF)
    Modified |= expandMBB(MBB);
  return Modified;
}

/// Returns an instance of the pseudo instruction expansion pass.
FunctionPass *llvm::createA64ExpandPseudoPass() {
  return new A64ExpandPseudo();
}
