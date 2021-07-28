//===-- A64Subtarget.cpp - A64 Subtarget Information ------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the A64 specific subclass of TargetSubtarget.
//
//===----------------------------------------------------------------------===//

#include "A64Subtarget.h"
#include "A64.h"
#include "llvm/Support/TargetParser.h"
using namespace llvm;

#define DEBUG_TYPE "a64-subtarget"

#define GET_SUBTARGETINFO_CTOR
#define GET_SUBTARGETINFO_TARGET_DESC
#include "A64GenSubtargetInfo.inc"

A64Subtarget::A64Subtarget(const Triple &TT, const std::string &CPU,
                           const std::string &FS, const TargetMachine &TM)
    : A64GenSubtargetInfo(TT, CPU, /*TuneCPU*/ CPU, FS),
      InstrInfo(initializeSubtargetDependencies(FS, CPU)), TLInfo(TM, *this) {}

A64Subtarget &
A64Subtarget::initializeSubtargetDependencies(StringRef FS,
                                              StringRef CPUString) {
  // Determine default and user-specified characteristics

  if (CPUString.empty())
    CPUString = "generic";

  ParseSubtargetFeatures(CPUString, /*TuneCPU*/ CPUString, FS);
  return *this;
}
