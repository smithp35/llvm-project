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
#include "A64.h"
#include "A64RegisterInfo.h"
#include "A64Subtarget.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/TargetLowering.h"

using namespace llvm;

A64TargetLowering::A64TargetLowering(const TargetMachine &TM,
                                     const A64Subtarget &STI)
    : TargetLowering(TM), Subtarget(&STI) {
  // Only support 64-bit registers in this backend.
  addRegisterClass(MVT::i64, &A64::GPR64allRegClass);

  // Must, computeRegisterProperties - Once all of the register classes are
  // added, this allows us to compute derived properties we expose.
  computeRegisterProperties(Subtarget->getRegisterInfo());

  setStackPointerRegisterToSaveRestore(A64::SP);

  // Use 0 or 1 for booleans
  setBooleanContents(ZeroOrOneBooleanContent);
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

//===----------------------------------------------------------------------===//
//                      Calling Convention Implementation
//===----------------------------------------------------------------------===//

#include "A64GenCallingConv.inc"

SDValue A64TargetLowering::LowerFormalArguments(
    SDValue Chain, CallingConv::ID CallConv, bool isVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &DL,
    SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const {
  MachineFunction &MF = DAG.getMachineFunction();
  MachineRegisterInfo &RegInfo = MF.getRegInfo();

  assert(!isVarArg && "VarArg not supported");
  // Assign locations to all of the incoming arguments.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(), ArgLocs,
                 *DAG.getContext());

  CCInfo.AnalyzeFormalArguments(Ins, CC_A64);

  for (auto &VA : ArgLocs) {
    if (VA.isRegLoc()) {
      // Arguments passed in registers
      EVT RegVT = VA.getLocVT();
      assert(RegVT.getSimpleVT().SimpleTy == MVT::i64 &&
             "Only support MVT::i64 register passing");
      const unsigned VReg = RegInfo.createVirtualRegister(&A64::GPR64RegClass);
      RegInfo.addLiveIn(VA.getLocReg(), VReg);
      SDValue ArgIn = DAG.getCopyFromReg(Chain, DL, VReg, RegVT);

      InVals.push_back(ArgIn);
      continue;
    }

    assert(VA.isMemLoc() &&
           "Can only pass arguments as either registers or via the stack");

    const unsigned Offset = VA.getLocMemOffset();

    const int FI = MF.getFrameInfo().CreateFixedObject(8, Offset, true);
    EVT PtrTy = getPointerTy(DAG.getDataLayout());
    SDValue FIPtr = DAG.getFrameIndex(FI, PtrTy);

    assert(VA.getValVT() == MVT::i64 &&
           "Only support passing arguments as i64");
    SDValue Load =
        DAG.getLoad(VA.getValVT(), DL, Chain, FIPtr, MachinePointerInfo());

    InVals.push_back(Load);
  }
  return Chain;
}

SDValue
A64TargetLowering::LowerReturn(SDValue Chain, CallingConv::ID CallConv,
                               bool isVarArg,
                               const SmallVectorImpl<ISD::OutputArg> &Outs,
                               const SmallVectorImpl<SDValue> &OutVals,
                               const SDLoc &DL, SelectionDAG &DAG) const {
  if (isVarArg) {
    report_fatal_error("VarArg not supported");
  }

  // CCValAssign - represent the assignment of
  // the return value to a location
  SmallVector<CCValAssign, 16> RVLocs;

  // CCState - Info about the registers and stack slot.
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(), RVLocs,
                 *DAG.getContext());

  CCInfo.AnalyzeReturn(Outs, RetCC_A64);

  SDValue Flag;
  SmallVector<SDValue, 8> RetOps(1, Chain);

  // Copy the result values into the output registers.
  for (unsigned i = 0, e = RVLocs.size(); i < e; ++i) {
    CCValAssign &VA = RVLocs[i];
    assert(VA.isRegLoc() && "Can only return in registers!");

    Chain = DAG.getCopyToReg(Chain, DL, VA.getLocReg(), OutVals[i], Flag);

    Flag = Chain.getValue(1);
    RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
  }

  RetOps[0] = Chain; // Update chain.

  // Add the flag if we have it.
  if (Flag.getNode()) {
    RetOps.push_back(Flag);
  }

  return DAG.getNode(A64ISD::RET_FLAG, DL, MVT::Other, RetOps);
}
