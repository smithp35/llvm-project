//===-- A64FixupKinds.h - A64 Specific Fixup Entries ------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_A64_MCTARGETDESC_A64FIXUPKINDS_H
#define LLVM_LIB_TARGET_A64_MCTARGETDESC_A64FIXUPKINDS_H

#include "llvm/MC/MCFixup.h"

namespace llvm {
namespace A64 {

enum Fixups {
  // Marker
  LastTargetFixupKind,
  NumTargetFixupKinds = LastTargetFixupKind - FirstTargetFixupKind
};

} // end namespace A64
} // end namespace llvm

#endif
