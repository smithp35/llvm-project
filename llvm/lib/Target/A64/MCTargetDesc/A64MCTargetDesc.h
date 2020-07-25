//===-- A64MCTargetDesc.h - A64 Target Descriptions -------------*- C++ -*-===//
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

#ifndef LLVM_LIB_TARGET_A64_MCTARGETDESC_A64MCTARGETDESC_H
#define LLVM_LIB_TARGET_A64_MCTARGETDESC_A64MCTARGETDESC_H

#include "llvm/MC/MCInstrDesc.h"
#include "llvm/Support/DataTypes.h"

#include <memory>

namespace llvm {
class MCAsmBackend;
class MCCodeEmitter;
class MCContext;
class MCInstrInfo;
class MCInstPrinter;
class MCObjectTargetWriter;
class MCRegisterInfo;
class MCStreamer;
class MCSubtargetInfo;
class MCTargetOptions;
class MCTargetStreamer;
class Target;
class formatted_raw_ostream;
class raw_ostream;

MCCodeEmitter *createA64MCCodeEmitter(const MCInstrInfo &MCII,
                                      MCContext &Ctx);

MCAsmBackend *createA64AsmBackend(const Target &T, const MCSubtargetInfo &STI,
                                  const MCRegisterInfo &MRI,
                                  const MCTargetOptions &Options);

MCTargetStreamer *createA64ObjectTargetStreamer(MCStreamer &S,
                                                const MCSubtargetInfo &STI);

MCTargetStreamer *createA64AsmTargetStreamer(MCStreamer &S,
                                             formatted_raw_ostream &OS,
                                             MCInstPrinter *InstPrint,
                                             bool isVerboseAsm);

std::unique_ptr<MCObjectTargetWriter> createA64ELFObjectWriter(uint8_t OSABI);

} // namespace llvm

// Defines symbolic names for A64 registers.  This defines a mapping from
// register name to register number.
//
#define GET_REGINFO_ENUM
#include "A64GenRegisterInfo.inc"

// Defines symbolic names for the LEG instructions.
//
#define GET_INSTRINFO_ENUM
#include "A64GenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "A64GenSubtargetInfo.inc"

#endif
