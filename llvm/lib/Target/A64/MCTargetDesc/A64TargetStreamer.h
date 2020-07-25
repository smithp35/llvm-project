//===-- A64TargetStreamer.h - A64 Target Streamer --------------*- C++ -*--===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_A64_MCTARGETDESC_A64TARGETSTREAMER_H
#define LLVM_LIB_TARGET_A64_MCTARGETDESC_A64TARGETSTREAMER_H

#include "llvm/MC/MCStreamer.h"

namespace {
class A64ELFStreamer;
}

namespace llvm {

class A64TargetStreamer : public MCTargetStreamer {
public:
  A64TargetStreamer(MCStreamer &S);
  ~A64TargetStreamer() override;

  void finish() override;
};

class A64TargetELFStreamer : public A64TargetStreamer {
private:
  A64ELFStreamer &getStreamer();

public:
  A64TargetELFStreamer(MCStreamer &S) : A64TargetStreamer(S) {}
};

MCTargetStreamer *createA64ObjectTargetStreamer(MCStreamer &S,
                                                const MCSubtargetInfo &STI);

} // end namespace llvm

#endif
