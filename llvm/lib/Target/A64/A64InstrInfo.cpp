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
#include "A64.h"
#include "A64RegisterInfo.h"
#include "A64Subtarget.h"
#include "MCTargetDesc/A64AddressingModes.h"
#include "Utils/A64BaseInfo.h"
#include "llvm/Support/Debug.h"

using namespace llvm;

#define GET_INSTRINFO_CTOR_DTOR
#include "A64GenInstrInfo.inc"

A64InstrInfo::A64InstrInfo(const A64Subtarget &STI)
    : A64GenInstrInfo(A64::ADJCALLSTACKDOWN, A64::ADJCALLSTACKUP),
      RI(STI.getTargetTriple()), Subtarget(STI) {}

void A64InstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                               MachineBasicBlock::iterator I,
                               const DebugLoc &DL, MCRegister DestReg,
                               MCRegister SrcReg, bool KillSrc) const {
  if (A64::GPR64spRegClass.contains(DestReg) &&
      (A64::GPR64spRegClass.contains(SrcReg) || SrcReg == A64::XZR)) {
    if (DestReg == A64::SP || SrcReg == A64::SP) {
      // If either operand is SP, expand to ADD #0.
      BuildMI(MBB, I, DL, get(A64::ADDiXri), DestReg)
          .addReg(SrcReg, getKillRegState(KillSrc))
          .addImm(0)
          .addImm(A64_AM::getShifterImm(A64_AM::LSL, 0));
    } else if (SrcReg == A64::XZR) {
      BuildMI(MBB, I, DL, get(A64::MOVZ), DestReg)
          .addImm(0)
          .addImm(A64_AM::getShifterImm(A64_AM::LSL, 0));
    }
    return;
  }
#ifndef NDEBUG
  const TargetRegisterInfo &TRI = getRegisterInfo();
  errs() << TRI.getRegAsmName(DestReg) << " = COPY "
         << TRI.getRegAsmName(SrcReg) << "\n";
#endif
  llvm_unreachable("unimplemented reg-to-reg copy");
}
