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
#include "MCTargetDesc/A64AddressingModes.h"
#include "Utils/A64BaseInfo.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/TargetLowering.h"
#include "llvm/Support/Debug.h"

using namespace llvm;

#define DEBUG_TYPE "a64-lower"

A64TargetLowering::A64TargetLowering(const TargetMachine &TM,
                                     const A64Subtarget &STI)
    : TargetLowering(TM), Subtarget(&STI) {
  // Only support 64-bit registers in this backend.
  addRegisterClass(MVT::i64, &A64::GPR64allRegClass);

  // Must, computeRegisterProperties - Once all of the register classes are
  // added, this allows us to compute derived properties we expose.
  computeRegisterProperties(Subtarget->getRegisterInfo());

  // Provide all sorts of operation actions
  setOperationAction(ISD::GlobalAddress, MVT::i64, Custom);
  setOperationAction(ISD::BR_CC, MVT::i64, Custom);
  setOperationAction(ISD::BRCOND, MVT::Other, Expand);
  setOperationAction(ISD::SETCC, MVT::i64, Custom);
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
  case A64ISD::ADRP:
    return "A64ISD::ADRP";
  case A64ISD::ADDlow:
    return "A64ISD::ADDlow";
  case A64ISD::RET_FLAG:
    return "A64ISD::RET_FLAG";
  case A64ISD::BRCOND:
    return "A64ISD::BRCOND";
  case A64ISD::ADDS:
    return "A64ISD::ADDS";
  case A64ISD::SUBS:
    return "A64ISD::SUBS";
  case A64ISD::CSEL:
    return "A64ISD::CSEL";
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

/// changeIntCCToAArch64CC - Convert a DAG integer condition code to an AArch64
/// CC
static A64CC::CondCode changeIntCCToA64CC(ISD::CondCode CC) {
  switch (CC) {
  default:
    llvm_unreachable("Unknown condition code!");
  case ISD::SETNE:
    return A64CC::NE;
  case ISD::SETEQ:
    return A64CC::EQ;
  case ISD::SETGT:
    return A64CC::GT;
  case ISD::SETGE:
    return A64CC::GE;
  case ISD::SETLT:
    return A64CC::LT;
  case ISD::SETLE:
    return A64CC::LE;
  case ISD::SETUGT:
    return A64CC::HI;
  case ISD::SETUGE:
    return A64CC::HS;
  case ISD::SETULT:
    return A64CC::LO;
  case ISD::SETULE:
    return A64CC::LS;
  }
}

static const MVT MVT_CC = MVT::i64;

static SDValue emitComparison(SDValue LHS, SDValue RHS, ISD::CondCode CC,
                              const SDLoc &dl, SelectionDAG &DAG) {
  EVT VT = LHS.getValueType();
  unsigned Opcode = A64ISD::SUBS;

  return DAG.getNode(Opcode, dl, DAG.getVTList(VT, MVT_CC), LHS, RHS)
      .getValue(1);
}

static SDValue getA64Cmp(SDValue LHS, SDValue RHS, ISD::CondCode CC,
                         SDValue &A64cc, SelectionDAG &DAG, const SDLoc &dl) {
  SDValue Cmp;
  A64CC::CondCode A64CC;
  Cmp = emitComparison(LHS, RHS, CC, dl, DAG);
  A64CC = changeIntCCToA64CC(CC);
  A64cc = DAG.getConstant(A64CC, dl, MVT_CC);
  return Cmp;
}

SDValue A64TargetLowering::LowerBR_CC(SDValue Op, SelectionDAG &DAG) const {
  SDValue Chain = Op.getOperand(0);
  ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(1))->get();
  SDValue LHS = Op.getOperand(2);
  SDValue RHS = Op.getOperand(3);
  SDValue Dest = Op.getOperand(4);
  SDLoc dl(Op);

  assert((LHS.getValueType() == RHS.getValueType()) &&
         (LHS.getValueType() == MVT::i64));
  SDValue CCVal;
  SDValue Cmp = getA64Cmp(LHS, RHS, CC, CCVal, DAG, dl);
  return DAG.getNode(A64ISD::BRCOND, dl, MVT::Other, Chain, Dest, CCVal, Cmp);
}

SDValue A64TargetLowering::LowerSETCC(SDValue Op, SelectionDAG &DAG) const {

  SDValue LHS = Op.getOperand(0);
  SDValue RHS = Op.getOperand(1);
  ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(2))->get();
  SDLoc dl(Op);

  // We chose ZeroOrOneBooleanContents, so use zero and one.
  EVT VT = Op.getValueType();
  SDValue TVal = DAG.getConstant(1, dl, VT);
  SDValue FVal = DAG.getConstant(0, dl, VT);

  assert(LHS.getValueType().isInteger());

  SDValue CCVal;
  SDValue Cmp = getA64Cmp(LHS, RHS, CC, CCVal, DAG, dl);

  SDValue Res = DAG.getNode(A64ISD::CSEL, dl, VT, TVal, FVal, CCVal, Cmp);
  return Res;
}

SDValue A64TargetLowering::LowerOperation(SDValue Op, SelectionDAG &DAG) const {
  LLVM_DEBUG(dbgs() << "Custom lowering: ");
  LLVM_DEBUG(Op.dump());
  switch (Op.getOpcode()) {
  default:
    llvm_unreachable("unimplemented operand");
    return SDValue();
  case ISD::BR_CC:
    return LowerBR_CC(Op, DAG);
  case ISD::SETCC:
    return LowerSETCC(Op, DAG);
  case ISD::GlobalAddress:
    return LowerGlobalAddress(Op, DAG);
  }
}

static SDValue getTargetNode(GlobalAddressSDNode *N, EVT Ty, SelectionDAG &DAG,
                             unsigned Flag) {
  return DAG.getTargetGlobalAddress(N->getGlobal(), SDLoc(N), Ty,
                                    N->getOffset(), Flag);
}

SDValue A64TargetLowering::LowerGlobalAddress(SDValue Op,
                                              SelectionDAG &DAG) const {
  GlobalAddressSDNode *GN = cast<GlobalAddressSDNode>(Op);
  SDLoc DL(GN);
  unsigned Flags = A64II::MO_NO_FLAG;
  EVT Ty = getPointerTy(DAG.getDataLayout());
  // We use the small code model of ADRP, ADD
  SDValue Hi = getTargetNode(GN, Ty, DAG, A64II::MO_PAGE | Flags);
  SDValue Lo =
      getTargetNode(GN, Ty, DAG, A64II::MO_PAGEOFF | A64II::MO_NC | Flags);
  SDValue ADRP = DAG.getNode(A64ISD::ADRP, DL, Ty, Hi);
  return DAG.getNode(A64ISD::ADDlow, DL, Ty, ADRP, Lo);
}

SDValue A64TargetLowering::PerformDAGCombine(SDNode *N,
                                             DAGCombinerInfo &DCI) const {
  SelectionDAG &DAG = DCI.DAG;
  switch (N->getOpcode()) {
  default:
    LLVM_DEBUG(dbgs() << "Custom combining: skipping\n");
    break;
  case A64ISD::BRCOND:
    // FIXME we can probably omit the expand.
    return SDValue();
  }
  return SDValue();
}
