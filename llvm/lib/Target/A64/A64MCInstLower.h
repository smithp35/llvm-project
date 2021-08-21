//===-- A64MCInstLower.h - Lower MachineInstr to MCInst ---------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_A64_A64MCINSTLOWER_H
#define LLVM_LIB_TARGET_A64_A64MCINSTLOWER_H

#include "llvm/ADT/Triple.h"
#include "llvm/Support/Compiler.h"

namespace llvm {
class MCContext;
class MCOperand;
class MCSymbol;
class MachineInstr;
class MachineOperand;
class MCInst;
class AsmPrinter;

/// A64MCInstLower - This class is used to lower an MachineInstr
/// into an MCInst.
class LLVM_LIBRARY_VISIBILITY A64MCInstLower {
  MCContext &Ctx;
  AsmPrinter &Printer;

public:
  A64MCInstLower(MCContext &ctx, AsmPrinter &printer);
  void Lower(const MachineInstr *MI, MCInst &OutMI) const;
  bool lowerOperand(const MachineOperand &MO, MCOperand &MCOp) const;
  MCOperand lowerSymbolOperand(const MachineOperand &MO, MCSymbol *Sym) const;
};
}

#endif
