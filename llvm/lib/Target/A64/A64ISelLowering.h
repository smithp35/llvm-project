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
  ADRP,     // Page address of a TargetGlobalAddress operand.
  ADDlow,   // Add the low 12 bits of a TargetGlobalAddress operand.
  RET_FLAG, // Return with a flag operand. Operand 0 is the chain operand.
  BRCOND,   // Conditional branch instruction; "b.cond".
  // Arithmetic instructions which write flags.
  ADDS,
  SUBS,
  // Conditional Select
  CSEL
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

  // Unconditionally 64-bit
  MVT getPointerTy(const DataLayout &DL, uint32_t AS = 0) const override {
    return MVT::getIntegerVT(64);
  }

  /// Provide custom lowering hooks for some operations.
  SDValue LowerOperation(SDValue Op, SelectionDAG &DAG) const override;

  SDValue PerformDAGCombine(SDNode *N, DAGCombinerInfo &DCI) const override;

  const char *getTargetNodeName(unsigned Opcode) const override;

private:
  /// Keep a pointer to the A64Subtarget around so that we can
  /// make the right decision when generating code for different targets.
  const A64Subtarget *Subtarget;

  SDValue LowerFormalArguments(SDValue Chain, CallingConv::ID CallConv,
                               bool isVarArg,
                               const SmallVectorImpl<ISD::InputArg> &Ins,
                               const SDLoc &DL, SelectionDAG &DAG,
                               SmallVectorImpl<SDValue> &InVals) const override;

  /// This hook must be implemented to lower outgoing return values,
  /// described by the Outs array, into the specified DAG. The
  /// implementation should return the resulting token chain value.
  SDValue LowerReturn(SDValue Chain, CallingConv::ID CallConv, bool isVarArg,
                      const SmallVectorImpl<ISD::OutputArg> &Outs,
                      const SmallVectorImpl<SDValue> &OutVals, const SDLoc &DL,
                      SelectionDAG &DAG) const override;

  SDValue LowerGlobalAddress(SDValue Op, SelectionDAG &DAG) const;

  SDValue LowerBR_CC(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerSETCC(SDValue Op, SelectionDAG &DAG) const;
};

} // namespace llvm
#endif
