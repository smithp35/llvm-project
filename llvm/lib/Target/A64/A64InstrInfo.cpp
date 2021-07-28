//===- A64InstrInfo.cpp - A64 Instruction Information -------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the A64 implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "A64InstrInfo.h"
#include "A64Subtarget.h"
using namespace llvm;

#define GET_INSTRINFO_CTOR_DTOR
#include "A64GenInstrInfo.inc"

A64InstrInfo::A64InstrInfo(const A64Subtarget &STI)
    : A64GenInstrInfo(A64::ADJCALLSTACKDOWN, A64::ADJCALLSTACKUP),
      Subtarget(STI) {}
