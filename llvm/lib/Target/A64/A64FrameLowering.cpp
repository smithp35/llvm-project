//===- A64FrameLowering.cpp - A64 Frame Lowering -------*- C++ -*-====//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===
#include "A64FrameLowering.h"
#include "A64.h"
#include "A64Subtarget.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"

using namespace llvm;

#define DEBUG_TYPE "frame-info"

void A64FrameLowering::emitPrologue(MachineFunction &MF,
                                    MachineBasicBlock &MBB) const {
  const MachineFrameInfo &MFI = MF.getFrameInfo();
  int64_t StackSize = MFI.getStackSize();
  unsigned StackAlign = getStackAlignment();
  if (StackAlign)
    StackSize = alignTo(StackSize, StackAlign);
  if (!StackSize)
    return;

  // Adjust SP
  const TargetInstrInfo &TII = *MF.getSubtarget().getInstrInfo();
  MachineBasicBlock::iterator MBBI = MBB.begin();
  DebugLoc dl = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();
  // If we have a frame pointer we need to update it
  if (hasFP(MF)) {
    BuildMI(MBB, MBBI, dl, TII.get(A64::SUBiXri), A64::FP)
        .addReg(A64::SP)
        .addImm(0)
        .addImm(0)
        .setMIFlag(MachineInstr::FrameSetup);
  }

  unsigned StackReg = A64::SP;
  // TODO Support sizes greater than the immediate in an ADD/SUB instruction
  BuildMI(MBB, MBBI, dl, TII.get(A64::SUBiXri), StackReg)
      .addReg(StackReg)
      .addImm(StackSize)
      .addImm(0)
      .setMIFlag(MachineInstr::FrameSetup);
}

void A64FrameLowering::emitEpilogue(MachineFunction &MF,
                                    MachineBasicBlock &MBB) const {
  const MachineFrameInfo &MFI = MF.getFrameInfo();
  int64_t StackSize = MFI.getStackSize();
  unsigned StackAlign = getStackAlignment();
  if (StackAlign)
    StackSize = alignTo(StackSize, StackAlign);
  if (!StackSize)
    return;

  // Restore SP to what it was at the beginning of the function.
  const TargetInstrInfo &TII = *MF.getSubtarget().getInstrInfo();
  MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr();
  DebugLoc dl = MBBI->getDebugLoc();
  unsigned StackReg = A64::SP;
  // TODO Support sizes greater than the immediate in an ADD/SUB instruction
  BuildMI(MBB, MBBI, dl, TII.get(A64::ADDiXri), StackReg)
      .addReg(StackReg)
      .addImm(StackSize)
      .addImm(0)
      .setMIFlag(MachineInstr::FrameSetup);

  // If we have a frame pointer we need to update it
  if (hasFP(MF)) {
    BuildMI(MBB, MBBI, dl, TII.get(A64::ADDiXri), A64::FP)
        .addReg(A64::SP)
        .addImm(0)
        .addImm(0)
        .setMIFlag(MachineInstr::FrameSetup);
  }
}

MachineBasicBlock::iterator A64FrameLowering::eliminateCallFramePseudoInstr(
    MachineFunction &MF, MachineBasicBlock &MBB,
    MachineBasicBlock::iterator I) const {
  return MBB.erase(I);
}

bool A64FrameLowering::hasFP(const MachineFunction &MF) const {
  return MF.getTarget().Options.DisableFramePointerElim(MF) ||
         MF.getFrameInfo().hasVarSizedObjects();
}
