//===-- A64ELFObjectWriter.cpp - A64 ELF Writer ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file handles ELF-specific object emission, converting LLVM's internal
// fixups into the appropriate relocations.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/A64FixupKinds.h"
#include "MCTargetDesc/A64MCExpr.h"
#include "MCTargetDesc/A64MCTargetDesc.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/ErrorHandling.h"
#include <cassert>
#include <cstdint>

using namespace llvm;

namespace {
class A64ELFObjectWriter : public MCELFObjectTargetWriter {
public:
  A64ELFObjectWriter(uint8_t OSABI);

  ~A64ELFObjectWriter() override = default;

protected:
  unsigned getRelocType(MCContext &Ctx, const MCValue &Target,
                        const MCFixup &Fixup, bool IsPCRel) const override;
};
} // End anonymous namespace

A64ELFObjectWriter::A64ELFObjectWriter(uint8_t OSABI)
    : MCELFObjectTargetWriter(/*Is64Bit*/ true, OSABI, ELF::EM_AARCH64,
                              /*HasRelocationAddend*/ true) {}

unsigned A64ELFObjectWriter::getRelocType(MCContext &Ctx, const MCValue &Target,
                                          const MCFixup &Fixup,
                                          bool IsPCRel) const {
  unsigned Kind = Fixup.getTargetKind();
  if (Kind >= FirstLiteralRelocationKind)
    return Kind - FirstLiteralRelocationKind;
  A64MCExpr::VariantKind RefKind =
      static_cast<A64MCExpr::VariantKind>(Target.getRefKind());
  A64MCExpr::VariantKind SymLoc = A64MCExpr::getSymbolLoc(RefKind);
  bool IsNC = A64MCExpr::isNotChecked(RefKind);

  assert((!Target.getSymA() ||
          Target.getSymA()->getKind() == MCSymbolRefExpr::VK_None ||
          Target.getSymA()->getKind() == MCSymbolRefExpr::VK_PLT) &&
         "Should only be expression-level modifiers here");

  assert((!Target.getSymB() ||
          Target.getSymB()->getKind() == MCSymbolRefExpr::VK_None) &&
         "Should only be expression-level modifiers here");

  if (IsPCRel) {
    switch (Kind) {
    case FK_Data_1:
      Ctx.reportError(Fixup.getLoc(), "1-byte data relocations not supported");
      return ELF::R_A64_NONE;
    case FK_Data_2:
      return ELF::R_A64_PREL16;
    case FK_Data_4:
      return ELF::R_A64_PREL32;
    case FK_Data_8:
      return ELF::R_A64_PREL64;
    case A64::fixup_a64_pcrel_adr_imm21:
      if (SymLoc != A64MCExpr::VK_ABS)
        Ctx.reportError(Fixup.getLoc(),
                        "invalid symbol kind for ADR relocation");
      return ELF::R_A64_ADR_PREL_LO21;
    case A64::fixup_a64_ldr_pcrel_imm19:
      return ELF::R_A64_LD_PREL_LO19;
    case A64::fixup_a64_pcrel_branch26:
      return ELF::R_A64_JUMP26;
    case A64::fixup_a64_pcrel_call26:
      return ELF::R_A64_CALL26;
    case A64::fixup_a64_pcrel_branch19:
      return ELF::R_A64_CONDBR19;
    default:
      Ctx.reportError(Fixup.getLoc(), "Unknown ELF relocation type");
      return ELF::R_A64_NONE;
    }
  } else {
    switch (Fixup.getTargetKind()) {
    case FK_Data_1:
      Ctx.reportError(Fixup.getLoc(), "1-byte data relocations not supported");
      return ELF::R_A64_NONE;
    case FK_Data_2:
      return ELF::R_A64_ABS16;
    case FK_Data_4:
      return ELF::R_A64_ABS32;
    case FK_Data_8:
      return ELF::R_A64_ABS64;
    case A64::fixup_a64_add_imm12:
      if (SymLoc == A64MCExpr::VK_ABS && IsNC)
        return ELF::R_A64_ADD_ABS_LO12_NC;
      Ctx.reportError(Fixup.getLoc(),
                      "invalid fixup for add (uimm12) instruction");
      return ELF::R_A64_NONE;
    default:
      Ctx.reportError(Fixup.getLoc(), "Unknown ELF relocation type");
      return ELF::R_A64_NONE;
    }
  }
  llvm_unreachable("Unimplemented fixup -> relocation");
}

std::unique_ptr<MCObjectTargetWriter>
llvm::createA64ELFObjectWriter(uint8_t OSABI) {
  return std::make_unique<A64ELFObjectWriter>(OSABI);
}
