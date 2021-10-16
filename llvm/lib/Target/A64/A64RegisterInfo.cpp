//===- A64RegisterInfo.cpp - A64 Register Information -------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the A64 implementation of the TargetRegisterInfo
// class.
//
//===----------------------------------------------------------------------===//

#include "A64RegisterInfo.h"
#include "A64FrameLowering.h"
#include "A64InstrInfo.h"
#include "llvm/CodeGen/MachineFrameInfo.h"

using namespace llvm;

#define GET_REGINFO_TARGET_DESC
#include "A64GenRegisterInfo.inc"

A64RegisterInfo::A64RegisterInfo(const Triple &TT)
    : A64GenRegisterInfo(A64::LR) {}

const uint16_t *
A64RegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  assert(MF && "Invalid MachineFunction pointer.");
  return CC_Save_SaveList;
}

const uint32_t *A64RegisterInfo::getCallPreservedMask(const MachineFunction &MF,
                                                      CallingConv::ID) const {
  return CC_Save_RegMask;
}

BitVector A64RegisterInfo::getReservedRegs(const MachineFunction &MF) const {

  BitVector Reserved(getNumRegs());
  markSuperRegs(Reserved, A64::SP);
  markSuperRegs(Reserved, A64::XZR);
  // Always reserve frame pointer
  markSuperRegs(Reserved, A64::FP);
  return Reserved;
}

bool A64RegisterInfo::requiresRegisterScavenging(
    const MachineFunction &MF) const {
  return true;
}

bool A64RegisterInfo::useFPForScavengingIndex(const MachineFunction &MF) const {
  return false;
}

// FrameIndexes are locations on the stack. Replace index with offset.
void A64RegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                          int SPAdj, unsigned FIOperandNum,
                                          RegScavenger *RS) const {
  MachineInstr &MI = *II;
  const MachineFunction &MF = *MI.getParent()->getParent();
  const MachineFrameInfo &MFI = MF.getFrameInfo();

  MachineOperand &FIOp = MI.getOperand(FIOperandNum);
  unsigned FI = FIOp.getIndex();

  // Determine if we can eliminate the index from this kind of instruction.
  unsigned ImmOpIdx = 0;
  switch (MI.getOpcode()) {
  default:
    // Not supported yet.
    assert(false && "MI not supported yet");
    return;
  case A64::LDRXui:
  case A64::STRXui:
    ImmOpIdx = FIOperandNum + 1;
    break;
  }
  MachineOperand &ImmOp = MI.getOperand(ImmOpIdx);
  // Calculate offset from frame index.
  int Offset = MFI.getObjectOffset(FI) + MFI.getStackSize() + ImmOp.getImm();
  FIOp.ChangeToRegister(A64::SP, false);
  // Offset for LDRXui is scaled by 8
  ImmOp.setImm(Offset / 8);
}

// Debug information queries.
Register A64RegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  const A64FrameLowering *TFI = getFrameLowering(MF);
  return TFI->hasFP(MF) ? A64::FP : A64::SP;
}
