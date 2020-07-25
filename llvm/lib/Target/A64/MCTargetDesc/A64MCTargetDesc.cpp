//===-- A64MCTargetDesc.cpp - A64 Target Descriptions -----------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file provides A64 specific target descriptions.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/A64MCTargetDesc.h"
#include "MCTargetDesc/A64ELFStreamer.h"
#include "MCTargetDesc/A64InstPrinter.h"
#include "MCTargetDesc/A64MCAsmInfo.h"
#include "TargetInfo/A64TargetInfo.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrDesc.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCValue.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

#define GET_INSTRINFO_MC_DESC
#define GET_INSTRINFO_MC_HELPERS
#include "A64GenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "A64GenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "A64GenRegisterInfo.inc"

static MCInstrInfo *createA64MCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitA64MCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createA64MCRegisterInfo(const Triple &Triple) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitA64MCRegisterInfo(X, A64::LR);
  return X;
}

static MCSubtargetInfo *createA64MCSubtargetInfo(const Triple &TT,
                                                 StringRef CPU, StringRef FS) {
  if (CPU.empty())
    CPU = "generic";

  return createA64MCSubtargetInfoImpl(TT, CPU, /*TuneCPU*/ CPU, FS);
}

static MCAsmInfo *createA64MCAsmInfo(const MCRegisterInfo &MRI,
                                     const Triple &TheTriple,
                                     const MCTargetOptions &Options) {
  MCAsmInfo *MAI;
  assert(TheTriple.isOSBinFormatELF() && "Invalid target");
  MAI = new A64MCAsmInfoELF(TheTriple);

  // Initial state of the frame pointer is SP.
  unsigned Reg = MRI.getDwarfRegNum(A64::SP, true);
  MCCFIInstruction Inst = MCCFIInstruction::cfiDefCfa(nullptr, Reg, 0);
  MAI->addInitialFrameState(Inst);

  return MAI;
}

static MCInstPrinter *createA64MCInstPrinter(const Triple &T,
                                             unsigned SyntaxVariant,
                                             const MCAsmInfo &MAI,
                                             const MCInstrInfo &MII,
                                             const MCRegisterInfo &MRI) {

  return new A64InstPrinter(MAI, MII, MRI);
}

static MCStreamer *createELFStreamer(const Triple &T, MCContext &Ctx,
                                     std::unique_ptr<MCAsmBackend> &&TAB,
                                     std::unique_ptr<MCObjectWriter> &&OW,
                                     std::unique_ptr<MCCodeEmitter> &&Emitter,
                                     bool RelaxAll) {
  return createA64ELFStreamer(Ctx, std::move(TAB), std::move(OW),
                              std::move(Emitter), RelaxAll);
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeA64TargetMC() {
  Target *T = &getTheA64Target();
  // Register the MC asm info.
  RegisterMCAsmInfoFn X(*T, createA64MCAsmInfo);

  // Register the MC instruction info.
  TargetRegistry::RegisterMCInstrInfo(*T, createA64MCInstrInfo);

  // Register the MC register info.
  TargetRegistry::RegisterMCRegInfo(*T, createA64MCRegisterInfo);

  // Register the MC subtarget info.
  TargetRegistry::RegisterMCSubtargetInfo(*T, createA64MCSubtargetInfo);

  // Register the MC Code Emitter
  TargetRegistry::RegisterMCCodeEmitter(*T, createA64MCCodeEmitter);

  // Register the obj streamers.
  TargetRegistry::RegisterELFStreamer(*T, createELFStreamer);

  // Register the obj target streamer.
  TargetRegistry::RegisterObjectTargetStreamer(*T,
                                               createA64ObjectTargetStreamer);

  // Register the asm streamer.
  TargetRegistry::RegisterAsmTargetStreamer(*T, createA64AsmTargetStreamer);
  // Register the MCInstPrinter.
  TargetRegistry::RegisterMCInstPrinter(*T, createA64MCInstPrinter);

  // Register the asm backend.
  TargetRegistry::RegisterMCAsmBackend(*T, createA64AsmBackend);
}
