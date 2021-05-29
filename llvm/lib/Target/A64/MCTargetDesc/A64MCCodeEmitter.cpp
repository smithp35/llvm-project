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
  // TODO deal with fixups
  // const MCExpr *Expr = MO.getExpr();
  // // Encode the 12 bits of the fixup.
  // MCFixupKind Kind = MCFixupKind(A64::fixup_a64_add_imm12);
  // Fixups.push_back(MCFixup::create(0, Expr, Kind, MI.getLoc()));

  // ++MCNumFixups;

  // // Set the shift bit of the add instruction for relocation types
  // // R_AARCH64_TLSLE_ADD_TPREL_HI12 and R_AARCH64_TLSLD_ADD_DTPREL_HI12.
  // if (const A64MCExpr *A64E = dyn_cast<A64MCExpr>(Expr)) {
  //   A64MCExpr::VariantKind RefKind = A64E->getKind();
  //   if (RefKind == A64MCExpr::VK_TPREL_HI12 ||
  //       RefKind == A64MCExpr::VK_DTPREL_HI12 ||
  //       RefKind == A64MCExpr::VK_SECREL_HI12)
  //     ShiftVal = 12;
  // }
  return ShiftVal == 0 ? 0 : (1 << ShiftVal);
}

uint32_t
A64MCCodeEmitter::getMoveWideImmOpValue(const MCInst &MI, unsigned OpIdx,
                                        SmallVectorImpl<MCFixup> &Fixups,
                                        const MCSubtargetInfo &STI) const {
  const MCOperand &MO = MI.getOperand(OpIdx);

  if (MO.isImm())
    return MO.getImm();
  assert(MO.isExpr() && "Unexpected movz/movk immediate");
  // TODO support fixups
  //  Fixups.push_back(MCFixup::create(
  //    0, MO.getExpr(), MCFixupKind(AArch64::fixup_aarch64_movw),
  //    MI.getLoc()));

  //++MCNumFixups;

  return 0;
}

#define ENABLE_INSTR_PREDICATE_VERIFIER
#include "A64GenMCCodeEmitter.inc"

MCCodeEmitter *llvm::createA64MCCodeEmitter(const MCInstrInfo &MCII,
                                            MCContext &Ctx) {
  return new A64MCCodeEmitter(MCII, Ctx);
}
