//===-- A64TargetObjectFile.h - A64 Object Info -*- C++ -----------------*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_A64_A64TARGETOBJECTFILE_H
#define LLVM_LIB_TARGET_A64_A64TARGETOBJECTFILE_H

#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/Target/TargetLoweringObjectFile.h"

namespace llvm {
class A64TargetMachine;

/// This implementation is used for A64 ELF targets (Linux in particular).
class A64_ELFTargetObjectFile : public TargetLoweringObjectFileELF {
  void Initialize(MCContext &Ctx, const TargetMachine &TM) override;

public:
  A64_ELFTargetObjectFile() {
    PLTRelativeVariantKind = MCSymbolRefExpr::VK_PLT;
  }
};

} // end namespace llvm
#endif
