//===-- A64SelectionDAGInfo.h - A64 SelectionDAG Info ---*- C++ --------*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines the A64 subclass for SelectionDAGTargetInfo.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_A64_A64SELECTIONDAGINFO_H
#define LLVM_LIB_TARGET_A64_A64SELECTIONDAGINFO_H

#include "llvm/CodeGen/SelectionDAGTargetInfo.h"

namespace llvm {

class A64SelectionDAGInfo : public SelectionDAGTargetInfo {
public:
  ~A64SelectionDAGInfo();
};
}

#endif
