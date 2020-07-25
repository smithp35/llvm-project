//===- A64TargetStreamer.cpp - A64TargetStreamer class --------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the A64TargetStreamer class.
//
//===----------------------------------------------------------------------===//

#include "A64TargetStreamer.h"

using namespace llvm;

//
// A64TargetStreamer Implemenation
//
A64TargetStreamer::A64TargetStreamer(MCStreamer &S) : MCTargetStreamer(S) {}

A64TargetStreamer::~A64TargetStreamer() = default;

// finish() - write out any non-empty assembler constant pools.
void A64TargetStreamer::finish() {}

namespace llvm {

MCTargetStreamer *createA64ObjectTargetStreamer(MCStreamer &S,
                                                const MCSubtargetInfo &STI) {
  return new A64TargetELFStreamer(S);
}

} // end namespace llvm
