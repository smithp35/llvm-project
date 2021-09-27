//===- A64InstrInfo.cpp - A64 Instruction Information -------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the A64 implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "A64InstrInfo.h"
#include "A64.h"
#include "A64RegisterInfo.h"
#include "A64Subtarget.h"
#include "MCTargetDesc/A64AddressingModes.h"
#include "Utils/A64BaseInfo.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/Support/Debug.h"

using namespace llvm;

#define GET_INSTRINFO_CTOR_DTOR
#include "A64GenInstrInfo.inc"

A64InstrInfo::A64InstrInfo(const A64Subtarget &STI)
    : A64GenInstrInfo(A64::ADJCALLSTACKDOWN, A64::ADJCALLSTACKUP),
      RI(STI.getTargetTriple()), Subtarget(STI) {}

void A64InstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                               MachineBasicBlock::iterator I,
                               const DebugLoc &DL, MCRegister DestReg,
                               MCRegister SrcReg, bool KillSrc) const {
  if (A64::GPR64spRegClass.contains(DestReg) &&
      (A64::GPR64spRegClass.contains(SrcReg) || SrcReg == A64::XZR)) {
    if (DestReg == A64::SP || SrcReg == A64::SP) {
      // If either operand is SP, expand to ADD #0.
      BuildMI(MBB, I, DL, get(A64::ADDiXri), DestReg)
          .addReg(SrcReg, getKillRegState(KillSrc))
          .addImm(0)
          .addImm(A64_AM::getShifterImm(A64_AM::LSL, 0));
    } else if (SrcReg == A64::XZR) {
      BuildMI(MBB, I, DL, get(A64::MOVZ), DestReg)
          .addImm(0)
          .addImm(A64_AM::getShifterImm(A64_AM::LSL, 0));
    } else {
      BuildMI(MBB, I, DL, get(A64::ORRrXrs), DestReg)
        .addReg(A64::XZR)
        .addReg(SrcReg, getKillRegState(KillSrc))
        .addImm(A64_AM::getShifterImm(A64_AM::LSL, 0));
    }
    return;
  }
#ifndef NDEBUG
  const TargetRegisterInfo &TRI = getRegisterInfo();
  errs() << TRI.getRegAsmName(DestReg) << " = COPY "
         << TRI.getRegAsmName(SrcReg) << "\n";
#endif
  llvm_unreachable("unimplemented reg-to-reg copy");
}

void A64InstrInfo::storeRegToStackSlot(
    MachineBasicBlock &MBB, MachineBasicBlock::iterator MBBI, Register SrcReg,
    bool isKill, int FI, const TargetRegisterClass *RC,
    const TargetRegisterInfo *TRI) const {

  MachineFunction &MF = *MBB.getParent();
  MachineFrameInfo &MFI = MF.getFrameInfo();

  MachinePointerInfo PtrInfo = MachinePointerInfo::getFixedStack(MF, FI);
  MachineMemOperand *MMO =
      MF.getMachineMemOperand(PtrInfo, MachineMemOperand::MOStore,
                              MFI.getObjectSize(FI), MFI.getObjectAlign(FI));

  assert(TRI->getSpillSize(*RC) == 8);
  unsigned Opc = A64::STRXui;
  if (Register::isVirtualRegister(SrcReg))
    MF.getRegInfo().constrainRegClass(SrcReg, &A64::GPR64RegClass);
  else
    assert(SrcReg != A64::SP);

  const MachineInstrBuilder MI = BuildMI(MBB, MBBI, DebugLoc(), get(Opc))
                                     .addReg(SrcReg, getKillRegState(isKill))
                                     .addFrameIndex(FI)
                                     .addImm(0)
                                     .addMemOperand(MMO);
}

void A64InstrInfo::loadRegFromStackSlot(
    MachineBasicBlock &MBB, MachineBasicBlock::iterator MBBI, Register DestReg,
    int FI, const TargetRegisterClass *RC,
    const TargetRegisterInfo *TRI) const {

  MachineFunction &MF = *MBB.getParent();
  MachineFrameInfo &MFI = MF.getFrameInfo();
  MachinePointerInfo PtrInfo = MachinePointerInfo::getFixedStack(MF, FI);
  MachineMemOperand *MMO =
      MF.getMachineMemOperand(PtrInfo, MachineMemOperand::MOLoad,
                              MFI.getObjectSize(FI), MFI.getObjectAlign(FI));

  assert(TRI->getSpillSize(*RC) == 8);
  unsigned Opc = A64::LDRXui;
  if (Register::isVirtualRegister(DestReg))
    MF.getRegInfo().constrainRegClass(DestReg, &A64::GPR64RegClass);
  else
    assert(DestReg != A64::SP);

  const MachineInstrBuilder MI = BuildMI(MBB, MBBI, DebugLoc(), get(Opc))
                                     .addReg(DestReg, getDefRegState(true))
                                     .addFrameIndex(FI)
                                     .addImm(0)
                                     .addMemOperand(MMO);
}
