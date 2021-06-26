//=--- A64MCExpr.h - AArch64 specific MC expression classes ---*- C++ ----*-=//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file describes AArch64-specific MCExprs, used for modifiers like
// ":lo12:" or ":gottprel_g1:".
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_A64_MCTARGETDESC_A64MCEXPR_H
#define LLVM_LIB_TARGET_A64_MCTARGETDESC_A64MCEXPR_H

#include "llvm/MC/MCExpr.h"
#include "llvm/Support/ErrorHandling.h"

namespace llvm {

class A64MCExpr : public MCTargetExpr {
public:
  enum VariantKind {
    // Symbol locations specifying (roughly speaking) what calculation should be
    // performed to construct the final address for the relocated
    // symbol. E.g. direct, via the GOT, ...
    VK_ABS = 0x001,
    VK_SymLocBits = 0x00f,

    // Variants specifying which part of the final address calculation is
    // used. E.g. the low 12 bits for an ADD/LDR, the middle 16 bits for a
    // MOVZ/MOVK.
    VK_PAGEOFF = 0x020,

    // Whether the final relocation is a checked one (where a linker should
    // perform a range-check on the final address) or not. Note that this field
    // is unfortunately sometimes omitted from the assembly syntax. E.g. :lo12:
    // on its own is a non-checked relocation. We side with ELF on being
    // explicit about this!
    VK_NC = 0x100,

    // Convenience definitions for referring to specific textual representations
    // of relocation specifiers. Note that this means the "_NC" is sometimes
    // omitted in line with assembly syntax here (VK_LO12 rather than VK_LO12_NC
    // since a user would write ":lo12:").
    VK_CALL              = VK_ABS,
    VK_LO12 = VK_ABS | VK_PAGEOFF | VK_NC,

    VK_INVALID = 0xfff
  };

private:
  const MCExpr *Expr;
  const VariantKind Kind;

  explicit A64MCExpr(const MCExpr *Expr, VariantKind Kind)
      : Expr(Expr), Kind(Kind) {}

public:
  /// @name Construction
  /// @{

  static const A64MCExpr *create(const MCExpr *Expr, VariantKind Kind,
                                 MCContext &Ctx);

  /// @}
  /// @name Accessors
  /// @{

  /// Get the kind of this expression.
  VariantKind getKind() const { return Kind; }

  /// Get the expression this modifier applies to.
  const MCExpr *getSubExpr() const { return Expr; }

  static VariantKind getSymbolLoc(VariantKind Kind) {
    return static_cast<VariantKind>(Kind & VK_SymLocBits);
  }

  static bool isNotChecked(VariantKind Kind) { return Kind & VK_NC; }

  /// Convert the variant kind into an ELF-appropriate modifier
  /// (e.g. ":got:", ":lo12:").
  StringRef getVariantKindName() const;

  void printImpl(raw_ostream &OS, const MCAsmInfo *MAI) const override;

  void visitUsedExpr(MCStreamer &Streamer) const override;

  MCFragment *findAssociatedFragment() const override;

  bool evaluateAsRelocatableImpl(MCValue &Res, const MCAsmLayout *Layout,
                                 const MCFixup *Fixup) const override;

  void fixELFSymbolsInTLSFixups(MCAssembler &Asm) const override;

  static bool classof(const MCExpr *E) {
    return E->getKind() == MCExpr::Target;
  }

  static bool classof(const A64MCExpr *) { return true; }
};
} // namespace llvm
#endif // LLVM_LIB_TARGET_A64_MCTARGETDESC_A64MCEXPR_H
