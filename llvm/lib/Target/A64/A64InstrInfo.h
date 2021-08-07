//===- A64InstrInfo.h - A64 Instruction Information -----*- C++ ---------*-===//
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

#ifndef LLVM_LIB_TARGET_A64_A64INSTRINFO_H
#define LLVM_LIB_TARGET_A64_A64INSTRINFO_H

#include "A64.h"
#include "A64RegisterInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "A64GenInstrInfo.inc"

namespace llvm {

class A64Subtarget;

class A64InstrInfo final : public A64GenInstrInfo {
  const A64RegisterInfo RI;
  const A64Subtarget &Subtarget;

public:
  explicit A64InstrInfo(const A64Subtarget &STI);

  /// getRegisterInfo - TargetInstrInfo is a superset of MRegister info.  As
  /// such, whenever a client has an instance of instruction info, it should
  /// always be able to get register info as well (through this method).
  const A64RegisterInfo &getRegisterInfo() const { return RI; }

#define GET_INSTRINFO_HELPER_DECLS
#include "A64GenInstrInfo.inc"
};

} // namespace llvm
#endif
