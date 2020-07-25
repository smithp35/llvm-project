//===-- A64ELFObjectWriter.cpp - A64 ELF Writer ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file handles ELF-specific object emission, converting LLVM's internal
// fixups into the appropriate relocations.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/A64MCTargetDesc.h"
#include "llvm/MC/MCELFObjectWriter.h"

using namespace llvm;

namespace {
class A64ELFObjectWriter : public MCELFObjectTargetWriter {
public:
  A64ELFObjectWriter(uint8_t OSABI);

  ~A64ELFObjectWriter() override = default;

protected:
  unsigned getRelocType(MCContext &Ctx, const MCValue &Target,
                        const MCFixup &Fixup, bool IsPCRel) const override;
};
} // End anonymous namespace

A64ELFObjectWriter::A64ELFObjectWriter(uint8_t OSABI)
    : MCELFObjectTargetWriter(/*Is64Bit*/ true, OSABI, ELF::EM_AARCH64,
                              /*HasRelocationAddend*/ true) {}

unsigned A64ELFObjectWriter::getRelocType(MCContext &Ctx, const MCValue &Target,
                                          const MCFixup &Fixup,
                                          bool IsPCRel) const {
  return 0;
}

std::unique_ptr<MCObjectTargetWriter>
llvm::createA64ELFObjectWriter(uint8_t OSABI) {
  return std::make_unique<A64ELFObjectWriter>(OSABI);
}
