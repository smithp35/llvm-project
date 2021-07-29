//==-- A64FrameLowering.h - TargetFrameLowering for A64 --*- C++ -*---------==//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
//
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_A64_A64FRAMELOWERING_H
#define LLVM_LIB_TARGET_A64_A64FRAMELOWERING_H

#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {

class A64FrameLowering : public TargetFrameLowering {
public:
  explicit A64FrameLowering()
      : TargetFrameLowering(StackGrowsDown, Align(16), 0, Align(16),
                            false /*StackRealignable*/) {}
  void emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const override;
  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

  MachineBasicBlock::iterator
  eliminateCallFramePseudoInstr(MachineFunction &MF, MachineBasicBlock &MBB,
                                MachineBasicBlock::iterator I) const override;

  bool hasFP(const MachineFunction &MF) const override;
};
} // namespace llvm
#endif
