//===- A64ExternalSymbolizer.cpp - Symbolizer for A64 -----------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "A64ExternalSymbolizer.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/Support/Format.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "aarch64-disassembler"

bool A64ExternalSymbolizer::tryAddingSymbolicOperand(
    MCInst &MI, raw_ostream &CommentStream, int64_t Value, uint64_t Address,
    bool IsBranch, uint64_t Offset, uint64_t OpSize, uint64_t InstSize) {
  return false;
}
