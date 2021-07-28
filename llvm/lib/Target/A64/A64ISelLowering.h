//==-- A64ISelLowering.h - A64 DAG Lowering Interface -----------*- C++ -*-==//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that A64 uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_A64_A64ISELLOWERING_H
#define LLVM_LIB_TARGET_A64_A64ISELLOWERING_H

#include "A64.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {

namespace A64ISD {

enum NodeType : unsigned {
  FIRST_NUMBER = ISD::BUILTIN_OP_END,
  CALL,     // Function call.
  RET_FLAG, // Return with a flag operand. Operand 0 is the chain operand.
};
}

class A64Subtarget;
class A64TargetMachine;

//===--------------------------------------------------------------------===//
// TargetLowering Implementation
//===--------------------------------------------------------------------===//
class A64TargetLowering : public TargetLowering {
public:
  explicit A64TargetLowering(const TargetMachine &TM, const A64Subtarget &STI);

private:
  /// Keep a pointer to the A64Subtarget around so that we can
  /// make the right decision when generating code for different targets.
  const A64Subtarget *Subtarget;
};

} // namespace llvm
#endif
