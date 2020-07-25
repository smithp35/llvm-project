//===- lib/MC/A64ELFStreamer.cpp - ELF Object Output for A64 --------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file assembles .s files and emits A64 ELF .o object files. Different
// from generic ELF streamer in emitting mapping symbols ($x and $d) to delimit
// regions of data and code.
//
//===----------------------------------------------------------------------===//

#include "A64TargetStreamer.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCELFStreamer.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {

class A64TargetAsmStreamer : public A64TargetStreamer {

public:
  A64TargetAsmStreamer(MCStreamer &S, formatted_raw_ostream &OS);
};

A64TargetAsmStreamer::A64TargetAsmStreamer(MCStreamer &S,
                                           formatted_raw_ostream &OS)
    : A64TargetStreamer(S) {}

class A64ELFStreamer : public MCELFStreamer {
public:
  A64ELFStreamer(MCContext &Context, std::unique_ptr<MCAsmBackend> TAB,
                 std::unique_ptr<MCObjectWriter> OW,
                 std::unique_ptr<MCCodeEmitter> Emitter)
      : MCELFStreamer(Context, std::move(TAB), std::move(OW),
                      std::move(Emitter)) {}
};

} // end anonymous namespace

namespace llvm {

A64ELFStreamer &A64TargetELFStreamer::getStreamer() {
  return static_cast<A64ELFStreamer &>(Streamer);
}

MCTargetStreamer *createA64AsmTargetStreamer(MCStreamer &S,
                                             formatted_raw_ostream &OS,
                                             MCInstPrinter *InstPrint,
                                             bool isVerboseAsm) {
  return new A64TargetAsmStreamer(S, OS);
}

MCELFStreamer *createA64ELFStreamer(MCContext &Context,
                                    std::unique_ptr<MCAsmBackend> TAB,
                                    std::unique_ptr<MCObjectWriter> OW,
                                    std::unique_ptr<MCCodeEmitter> Emitter,
                                    bool RelaxAll) {
  A64ELFStreamer *S = new A64ELFStreamer(Context, std::move(TAB), std::move(OW),
                                         std::move(Emitter));
  if (RelaxAll)
    S->getAssembler().setRelaxAll(true);
  return S;
}

} // end namespace llvm
