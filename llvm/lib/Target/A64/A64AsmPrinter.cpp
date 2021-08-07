//===- A64AsmPrinter.cpp - A64 LLVM assembly writer ----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to the A64 assembly language.
//
//===----------------------------------------------------------------------===//

#include "A64.h"
#include "A64MCInstLower.h"
#include "TargetInfo/A64TargetInfo.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Target/TargetMachine.h"

using namespace llvm;

#define DEBUG_TYPE "asm-printer"

namespace {

class A64AsmPrinter : public AsmPrinter {
  A64MCInstLower MCInstLowering;

public:
  A64AsmPrinter(TargetMachine &TM, std::unique_ptr<MCStreamer> Streamer)
      : AsmPrinter(TM, std::move(Streamer)), MCInstLowering(OutContext, *this) {
  }

  StringRef getPassName() const override { return "A64 Assembly Printer"; }

  void emitInstruction(const MachineInstr *MI) override;
};
} // namespace

void A64AsmPrinter::emitInstruction(const MachineInstr *MI) {
  MCInst TmpInst;
  MCInstLowering.Lower(MI, TmpInst);

  EmitToStreamer(*OutStreamer, TmpInst);
}

// Force static initialization.
extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeA64AsmPrinter() {
  RegisterAsmPrinter<A64AsmPrinter> X(getTheA64Target());
}
