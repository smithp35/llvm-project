//=- A64/A64MCCodeEmitter.cpp - Convert A64 code to machine code-------------=//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the A64MCCodeEmitter class.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/A64AddressingModes.h"
#include "MCTargetDesc/A64FixupKinds.h"
#include "MCTargetDesc/A64MCTargetDesc.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/Endian.h"
#include "llvm/Support/EndianStream.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "mccodeemitter"

STATISTIC(MCNumEmitted, "Number of MC instructions emitted.");
STATISTIC(MCNumFixups, "Number of MC fixups created.");

namespace {
class A64MCCodeEmitter : public MCCodeEmitter {
  A64MCCodeEmitter(const A64MCCodeEmitter &) = delete;
  void operator=(const A64MCCodeEmitter &) = delete;
  const MCInstrInfo &MCII;
  const MCContext &CTX;

public:
  A64MCCodeEmitter(const MCInstrInfo &MCII, MCContext &CTX)
      : MCII(MCII), CTX(CTX) {}

  ~A64MCCodeEmitter() {}

  // getBinaryCodeForInstr - TableGen'erated function for getting the
  // binary encoding for an instruction.
  uint64_t getBinaryCodeForInstr(const MCInst &MI,
                                 SmallVectorImpl<MCFixup> &Fixups,
                                 const MCSubtargetInfo &STI) const;

  /// getMachineOpValue - Return binary encoding of operand. If the machine
  /// operand requires relocation, record the relocation and return zero.
  unsigned getMachineOpValue(const MCInst &MI, const MCOperand &MO,
                             SmallVectorImpl<MCFixup> &Fixups,
                             const MCSubtargetInfo &STI) const;

  /// getAddSubImmOpValue - Return encoding for the 12-bit immediate value and
  /// the 2-bit shift field.
  uint32_t getAddSubImmOpValue(const MCInst &MI, unsigned OpIdx,
                               SmallVectorImpl<MCFixup> &Fixups,
                               const MCSubtargetInfo &STI) const;

  /// getAdrLabelOpValue - Return encoding info for 21-bit immediate ADR label
  /// target.
  uint32_t getAdrLabelOpValue(const MCInst &MI, unsigned OpIdx,
                              SmallVectorImpl<MCFixup> &Fixups,
                              const MCSubtargetInfo &STI) const;

  /// getBranchTargetOpValue - Return the encoded value for an unconditional
  /// branch target.
  uint32_t getBranchTargetOpValue(const MCInst &MI, unsigned OpIdx,
                                  SmallVectorImpl<MCFixup> &Fixups,
                                  const MCSubtargetInfo &STI) const;

  /// getCondBranchTargetOpValue - Return the encoded value for a conditional
  /// branch target.
  uint32_t getCondBranchTargetOpValue(const MCInst &MI, unsigned OpIdx,
                                      SmallVectorImpl<MCFixup> &Fixups,
                                      const MCSubtargetInfo &STI) const;

  /// getLoadLiteralOpValue - Return the encoded value for a load-literal
  /// pc-relative address.
  uint32_t getLoadLiteralOpValue(const MCInst &MI, unsigned OpIdx,
                                 SmallVectorImpl<MCFixup> &Fixups,
                                 const MCSubtargetInfo &STI) const;

  /// getLdStUImm12OpValue - Return encoding info for 12-bit unsigned immediate
  /// attached to a load, store
  /// TODO support fixups.
  uint32_t getLdStUImm12OpValue(const MCInst &MI, unsigned OpIdx,
                                SmallVectorImpl<MCFixup> &Fixups,
                                const MCSubtargetInfo &STI) const;

  /// getMoveWideImmOpValue - Return the encoded value for the immediate operand
  /// of a MOVZ or MOVK instruction.
  uint32_t getMoveWideImmOpValue(const MCInst &MI, unsigned OpIdx,
                                 SmallVectorImpl<MCFixup> &Fixups,
                                 const MCSubtargetInfo &STI) const;

  void encodeInstruction(const MCInst &MI, raw_ostream &OS,
                         SmallVectorImpl<MCFixup> &Fixups,
                         const MCSubtargetInfo &STI) const override;

private:
  FeatureBitset computeAvailableFeatures(const FeatureBitset &FB) const;
  void
  verifyInstructionPredicates(const MCInst &MI,
                              const FeatureBitset &AvailableFeatures) const;
};

void A64MCCodeEmitter::encodeInstruction(const MCInst &MI, raw_ostream &OS,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  uint64_t Binary = getBinaryCodeForInstr(MI, Fixups, STI);
  support::endian::write<uint32_t>(OS, Binary, support::little);
  ++MCNumEmitted; // Keep track of the # of mi's emitted.
}

} // end anonymous namespace

/// getMachineOpValue - Return binary encoding of operand. If the machine
/// operand requires relocation, record the relocation and return zero.
unsigned A64MCCodeEmitter::getMachineOpValue(const MCInst &MI,
                                             const MCOperand &MO,
                                             SmallVectorImpl<MCFixup> &Fixups,
                                             const MCSubtargetInfo &STI) const {
  if (MO.isReg())
    return CTX.getRegisterInfo()->getEncodingValue(MO.getReg());

  assert(MO.isImm() && "did not expect relocated expression");
  return static_cast<unsigned>(MO.getImm());
}

/// getAddSubImmOpValue - Return encoding for the 12-bit immediate value and
/// the 2-bit shift field.  The shift field is stored in bits 13-14 of the
/// return value.
uint32_t
A64MCCodeEmitter::getAddSubImmOpValue(const MCInst &MI, unsigned OpIdx,
                                      SmallVectorImpl<MCFixup> &Fixups,
                                      const MCSubtargetInfo &STI) const {
  // Suboperands are [imm, shifter].
  const MCOperand &MO = MI.getOperand(OpIdx);
  const MCOperand &MO1 = MI.getOperand(OpIdx + 1);
  assert(A64_AM::getShiftType(MO1.getImm()) == A64_AM::LSL &&
         "unexpected shift type for add/sub immediate");
  unsigned ShiftVal = A64_AM::getShiftValue(MO1.getImm());
  assert((ShiftVal == 0 || ShiftVal == 12) &&
         "unexpected shift value for add/sub immediate");
  if (MO.isImm())
    return MO.getImm() | (ShiftVal == 0 ? 0 : (1 << ShiftVal));
  assert(MO.isExpr() && "Unable to encode MCOperand!");
  const MCExpr *Expr = MO.getExpr();

  // Encode the 12 bits of the fixup.
  MCFixupKind Kind = MCFixupKind(A64::fixup_a64_add_imm12);
  Fixups.push_back(MCFixup::create(0, Expr, Kind, MI.getLoc()));

  ++MCNumFixups;

  // No support for TLS and secrel

  return ShiftVal == 0 ? 0 : (1 << ShiftVal);
}

/// getAdrLabelOpValue - Return encoding info for 21-bit immediate ADR label
/// target.
uint32_t
A64MCCodeEmitter::getAdrLabelOpValue(const MCInst &MI, unsigned OpIdx,
                                     SmallVectorImpl<MCFixup> &Fixups,
                                     const MCSubtargetInfo &STI) const {
  const MCOperand &MO = MI.getOperand(OpIdx);

  // If the destination is an immediate, we have nothing to do.
  if (MO.isImm())
    return MO.getImm();
  assert(MO.isExpr() && "Unexpected target type!");
  const MCExpr *Expr = MO.getExpr();

  MCFixupKind Kind = MI.getOpcode() == A64::ADR
                         ? MCFixupKind(A64::fixup_a64_pcrel_adr_imm21)
                         : MCFixupKind(A64::fixup_a64_pcrel_adrp_imm21);
  Fixups.push_back(MCFixup::create(0, Expr, Kind, MI.getLoc()));

  MCNumFixups += 1;

  // All of the information is in the fixup.
  return 0;
}

/// getBranchTargetOpValue - Return the encoded value for an unconditional
/// branch target.
uint32_t
A64MCCodeEmitter::getBranchTargetOpValue(const MCInst &MI, unsigned OpIdx,
                                         SmallVectorImpl<MCFixup> &Fixups,
                                         const MCSubtargetInfo &STI) const {
  const MCOperand &MO = MI.getOperand(OpIdx);

  // If the destination is an immediate, we have nothing to do.
  if (MO.isImm())
    return MO.getImm();
  assert(MO.isExpr() && "Unexpected ADR target type!");

  MCFixupKind Kind = MI.getOpcode() == A64::BL
                         ? MCFixupKind(A64::fixup_a64_pcrel_call26)
                         : MCFixupKind(A64::fixup_a64_pcrel_branch26);
  Fixups.push_back(MCFixup::create(0, MO.getExpr(), Kind, MI.getLoc()));

  ++MCNumFixups;

  // All of the information is in the fixup.
  return 0;
}

/// getCondBranchTargetOpValue - Return the encoded value for a conditional
/// branch target.
uint32_t
A64MCCodeEmitter::getCondBranchTargetOpValue(const MCInst &MI, unsigned OpIdx,
                                             SmallVectorImpl<MCFixup> &Fixups,
                                             const MCSubtargetInfo &STI) const {
  const MCOperand &MO = MI.getOperand(OpIdx);

  // If the destination is an immediate, we have nothing to do.
  if (MO.isImm())
    return MO.getImm();
  assert(MO.isExpr() && "Unexpected target type!");

  MCFixupKind Kind = MCFixupKind(A64::fixup_a64_pcrel_branch19);
  Fixups.push_back(MCFixup::create(0, MO.getExpr(), Kind, MI.getLoc()));

  ++MCNumFixups;

  // All of the information is in the fixup.
  return 0;
}

/// getLoadLiteralOpValue - Return the encoded value for a load-literal
/// pc-relative address.
uint32_t
A64MCCodeEmitter::getLoadLiteralOpValue(const MCInst &MI, unsigned OpIdx,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  const MCOperand &MO = MI.getOperand(OpIdx);

  // If the destination is an immediate, we have nothing to do.
  if (MO.isImm())
    return MO.getImm();
  assert(MO.isExpr() && "Unexpected target type!");

  MCFixupKind Kind = MCFixupKind(A64::fixup_a64_ldr_pcrel_imm19);
  Fixups.push_back(MCFixup::create(0, MO.getExpr(), Kind, MI.getLoc()));

  ++MCNumFixups;

  // All of the information is in the fixup.
  return 0;
}

uint32_t
A64MCCodeEmitter::getLdStUImm12OpValue(const MCInst &MI, unsigned OpIdx,
                                       SmallVectorImpl<MCFixup> &Fixups,
                                       const MCSubtargetInfo &STI) const {
  const MCOperand &MO = MI.getOperand(OpIdx);
  uint32_t ImmVal = 0;

  if (MO.isImm())
    ImmVal = static_cast<uint32_t>(MO.getImm());
  else {
    assert(MO.isExpr() && "unable to encode load/store imm operand");
    MCFixupKind Kind = MCFixupKind(A64::fixup_a64_ldst_imm12_scale8);
    Fixups.push_back(MCFixup::create(0, MO.getExpr(), Kind, MI.getLoc()));
    ++MCNumFixups;
  }

  return ImmVal;
}

uint32_t
A64MCCodeEmitter::getMoveWideImmOpValue(const MCInst &MI, unsigned OpIdx,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  const MCOperand &MO = MI.getOperand(OpIdx);

  if (MO.isImm())
    return MO.getImm();
  assert(MO.isExpr() && "Unexpected movz/movk immediate");

  Fixups.push_back(MCFixup::create(
      0, MO.getExpr(), MCFixupKind(A64::fixup_a64_movw), MI.getLoc()));

  ++MCNumFixups;

  return 0;
}

#define ENABLE_INSTR_PREDICATE_VERIFIER
#include "A64GenMCCodeEmitter.inc"

MCCodeEmitter *llvm::createA64MCCodeEmitter(const MCInstrInfo &MCII,
                                            MCContext &Ctx) {
  return new A64MCCodeEmitter(MCII, Ctx);
}
