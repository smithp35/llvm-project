//===-- A64TargetInfo.h - A64 Target Implementation -------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_A64_TARGETINFO_A64TARGETINFO_H
#define LLVM_LIB_TARGET_A64_TARGETINFO_A64TARGETINFO_H

namespace llvm {

class Target;

Target &getTheA64Target();

} // namespace llvm

#endif
