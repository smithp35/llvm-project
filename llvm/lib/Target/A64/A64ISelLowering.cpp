//===-- A64ISelLowering.cpp - A64 DAG Lowering Implementation  ------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the A64TargetLowering class.
//
//===----------------------------------------------------------------------===//

#include "A64ISelLowering.h"
#include "A64Subtarget.h"
#include "A64RegisterInfo.h"
#include "llvm/CodeGen/TargetLowering.h"

using namespace llvm;

A64TargetLowering::A64TargetLowering(const TargetMachine &TM,
                                     const A64Subtarget &STI)
    : TargetLowering(TM), Subtarget(&STI) {
  // Only support 64-bit registers in this backend.
  addRegisterClass(MVT::i64, &A64::GPR64allRegClass);
}

const char *A64TargetLowering::getTargetNodeName(unsigned Opcode) const {
  switch ((A64ISD::NodeType)Opcode) {
  case A64ISD::FIRST_NUMBER:
    break;
  case A64ISD::CALL:
    return "A64ISD::CALL";
  case A64ISD::RET_FLAG:
    return "A64ISD::RET_FLAG";
  }
  return nullptr;
}
