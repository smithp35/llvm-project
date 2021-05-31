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
