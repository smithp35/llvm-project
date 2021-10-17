//=====-- A64MCAsmInfo.h - A64 asm properties -----------------*- C++ -*--====//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the A64MCAsmInfo class.
// The A64MCAsmInfo sets properties such as pointer size, what
// The comment string is.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_A64_MCTARGETDESC_A64MCASMINFO_H
#define LLVM_LIB_TARGET_A64_MCTARGETDESC_A64MCASMINFO_H

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {
class Triple;

struct A64MCAsmInfoELF : public MCAsmInfoELF {
  explicit A64MCAsmInfoELF(const Triple &T);
};

} // end namespace llvm

#endif
