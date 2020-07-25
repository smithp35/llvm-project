//===-- A64ELFStreamer.h - ELF Streamer for A64 -----------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements ELF streamer information for the A64 backend.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_A64_MCTARGETDESC_A64ELFSTREAMER_H
#define LLVM_LIB_TARGET_A64_MCTARGETDESC_A64ELFSTREAMER_H

#include "llvm/MC/MCELFStreamer.h"

namespace llvm {

MCELFStreamer *createA64ELFStreamer(MCContext &Context,
                                    std::unique_ptr<MCAsmBackend> TAB,
                                    std::unique_ptr<MCObjectWriter> OW,
                                    std::unique_ptr<MCCodeEmitter> Emitter,
                                    bool RelaxAll);
}

#endif
