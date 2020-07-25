//===-- A64TargetInfo.cpp - A64 Target Implementation ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
#include "TargetInfo/A64TargetInfo.h"
#include "A64.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

Target &llvm::getTheA64Target() {
  static Target TheA64Target;
  return TheA64Target;
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeA64TargetInfo() {
  RegisterTarget<Triple::a64, /*HasJIT=*/true> X(getTheA64Target(), "a64",
                                                 "A64 (little endian)", "A64");
}
