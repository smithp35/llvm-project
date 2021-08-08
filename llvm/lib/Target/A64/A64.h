//==-- A64.h - Top-level interface for A64  ----------------------*- C++ -*-==//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in the LLVM
// A64 back-end.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_A64_A64_H
#define LLVM_LIB_TARGET_A64_A64_H

#include "A64.h"
#include "MCTargetDesc/A64MCTargetDesc.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/PassRegistry.h"
#include "llvm/MC/SubtargetFeature.h"
#include "llvm/Support/DataTypes.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
class A64TargetMachine;
class FunctionPass;

FunctionPass *createA64ISelDag(A64TargetMachine &TM,
                               CodeGenOpt::Level OptLevel);
FunctionPass *createA64ExpandPseudoPass();

void initializeA64ExpandPseudoPass(PassRegistry&);

} // namespace llvm

#endif
