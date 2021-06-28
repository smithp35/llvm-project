//===-- A64MCExpr.cpp - A64 specific MC expression classes ---------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the implementation of the assembly expression modifiers
// accepted by the A64 architecture (e.g. ":lo12:", ":gottprel_g1:", ...).
//
//===----------------------------------------------------------------------===//

#include "A64MCExpr.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbolELF.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

#define DEBUG_TYPE "a64symbolrefexpr"

const A64MCExpr *A64MCExpr::create(const MCExpr *Expr, VariantKind Kind,
                                   MCContext &Ctx) {
  return new (Ctx) A64MCExpr(Expr, Kind);
}

StringRef A64MCExpr::getVariantKindName() const {
  switch (static_cast<uint32_t>(getKind())) {
  case VK_CALL:                return "";
  case VK_LO12:                return ":lo12:";
  case VK_ABS_G3:              return ":abs_g3:";
  case VK_ABS_G2:              return ":abs_g2:";
  case VK_ABS_G2_S:            return ":abs_g2_s:";
  case VK_ABS_G2_NC:           return ":abs_g2_nc:";
  case VK_ABS_G1:              return ":abs_g1:";
  case VK_ABS_G1_S:            return ":abs_g1_s:";
  case VK_ABS_G1_NC:           return ":abs_g1_nc:";
  case VK_ABS_G0:              return ":abs_g0:";
  case VK_ABS_G0_S:            return ":abs_g0_s:";
  case VK_ABS_G0_NC:           return ":abs_g0_nc:";
  default:
    llvm_unreachable("Invalid ELF symbol kind");
  }
  return "";
}

void A64MCExpr::printImpl(raw_ostream &OS, const MCAsmInfo *MAI) const {
  OS << getVariantKindName();
  Expr->print(OS, MAI);
}

void A64MCExpr::visitUsedExpr(MCStreamer &Streamer) const {
  Streamer.visitUsedExpr(*getSubExpr());
}

MCFragment *A64MCExpr::findAssociatedFragment() const {
  llvm_unreachable("FIXME: what goes here?");
}

bool A64MCExpr::evaluateAsRelocatableImpl(MCValue &Res,
                                          const MCAsmLayout *Layout,
                                          const MCFixup *Fixup) const {
  if (!getSubExpr()->evaluateAsRelocatable(Res, Layout, Fixup))
    return false;

  Res =
      MCValue::get(Res.getSymA(), Res.getSymB(), Res.getConstant(), getKind());

  return true;
}

// TODO TLS not implemented yet
void A64MCExpr::fixELFSymbolsInTLSFixups(MCAssembler &Asm) const { return; }
