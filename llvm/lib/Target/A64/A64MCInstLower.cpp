//==-- A64MCInstLower.cpp - Convert A64 MachineInstr to an MCInst ---------==//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains code to lower A64 MachineInstrs to their corresponding
// MCInst records.
//
//===----------------------------------------------------------------------===//

#include "A64MCInstLower.h"
#include "MCTargetDesc/A64MCExpr.h"
#include "Utils/A64BaseInfo.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCSymbol.h"

using namespace llvm;

A64MCInstLower::A64MCInstLower(MCContext &ctx, AsmPrinter &printer)
    : Ctx(ctx), Printer(printer) {}

MCOperand A64MCInstLower::lowerSymbolOperand(const MachineOperand &MO,
                                             MCSymbol *Sym) const {
  uint32_t RefFlags = 0;

  // No modifier means this is a generic reference, classified as absolute for
  // the cases where it matters (:abs_g0: etc).
  RefFlags |= A64MCExpr::VK_ABS;

  // As used ADRP, ADD sequence
  if ((MO.getTargetFlags() & A64II::MO_FRAGMENT) == A64II::MO_PAGE)
    RefFlags |= A64MCExpr::VK_PAGE;
  else if ((MO.getTargetFlags() & A64II::MO_FRAGMENT) == A64II::MO_PAGEOFF)
    RefFlags |= A64MCExpr::VK_PAGEOFF;

  // Non checking
  if (MO.getTargetFlags() & A64II::MO_NC)
    RefFlags |= A64MCExpr::VK_NC;

  // Symbol reference
  const MCExpr *Expr =
      MCSymbolRefExpr::create(Sym, MCSymbolRefExpr::VK_None, Ctx);
  if (MO.getOffset())
    // reference + offset form
    Expr = MCBinaryExpr::createAdd(
        Expr, MCConstantExpr::create(MO.getOffset(), Ctx), Ctx);

  A64MCExpr::VariantKind RefKind;
  RefKind = static_cast<A64MCExpr::VariantKind>(RefFlags);
  Expr = A64MCExpr::create(Expr, RefKind, Ctx);

  return MCOperand::createExpr(Expr);
}

bool A64MCInstLower::lowerOperand(const MachineOperand &MO,
                                  MCOperand &MCOp) const {
  switch (MO.getType()) {
  default:
    // Unexpected operand, most likely we haven't implemented
    // it yet. Dump its contents so we can see what needs
    // implementing.
    MO.dump();
    llvm_unreachable("unknown operand type");
  case MachineOperand::MO_MachineBasicBlock:
    // Reference basic block with its symbol.
    MCOp = MCOperand::createExpr(
        MCSymbolRefExpr::create(MO.getMBB()->getSymbol(), Ctx));
    break;
  case MachineOperand::MO_Register:
    // Create equivalent MCInst register for Operand.
    // Ignore all implicit register operands. These don't
    // exist in MC
    if (MO.isImplicit())
      return false;
    MCOp = MCOperand::createReg(MO.getReg());
    break;
  case MachineOperand::MO_Immediate:
    // Create equivalent MCInst immediate for Operand.
    MCOp = MCOperand::createImm(MO.getImm());
    break;
  case MachineOperand::MO_GlobalAddress: {
    // Global address is a symbol reference.
    // Performed by helper function.
    const GlobalValue *GV = MO.getGlobal();
    MCOp = lowerSymbolOperand(MO, Printer.getSymbolPreferLocal(*GV));
    break;
  }
  case MachineOperand::MO_RegisterMask:
    // As used in a CALL
    // No equivalent in MCInst.
    break;
  }
  return true;
}

void A64MCInstLower::Lower(const MachineInstr *MI, MCInst &OutMI) const {
  // Opcodes are the same
  OutMI.setOpcode(MI->getOpcode());

  // If MachineOperand has a lowered equivalent, add it.
  for (const MachineOperand &MO : MI->operands()) {
    MCOperand MCOp;
    if (lowerOperand(MO, MCOp))
      OutMI.addOperand(MCOp);
  }
}
