//===-- A64AsmBackend.cpp - A64 Assembler Backend -------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/A64FixupKinds.h"
#include "MCTargetDesc/A64MCTargetDesc.h"
#include "llvm/ADT/Triple.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/Support/EndianStream.h"

using namespace llvm;

namespace {

class A64AsmBackend : public MCAsmBackend {
protected:
  Triple TheTriple;

public:
  A64AsmBackend(const Target &T, const Triple &TT)
      : MCAsmBackend(support::little), TheTriple(TT) {}

  unsigned getPointerSize() const { return 8; }

  unsigned getNumFixupKinds() const override {
    return A64::NumTargetFixupKinds;
  }

  void applyFixup(const MCAssembler &Asm, const MCFixup &Fixup,
                  const MCValue &Target, MutableArrayRef<char> Data,
                  uint64_t Value, bool IsResolved,
                  const MCSubtargetInfo *STI) const override;

  bool mayNeedRelaxation(const MCInst &Inst,
                         const MCSubtargetInfo &STI) const override;

  bool fixupNeedsRelaxation(const MCFixup &Fixup, uint64_t Value,
                            const MCRelaxableFragment *DF,
                            const MCAsmLayout &Layout) const override;
  void relaxInstruction(MCInst &Inst,
                        const MCSubtargetInfo &STI) const override;
  bool writeNopData(raw_ostream &OS, uint64_t Count,
                    const MCSubtargetInfo *STI) const override;
};

} // end anonymous namespace

void A64AsmBackend::applyFixup(const MCAssembler &Asm, const MCFixup &Fixup,
                               const MCValue &Target,
                               MutableArrayRef<char> Data, uint64_t Value,
                               bool IsResolved,
                               const MCSubtargetInfo *STI) const {}

bool A64AsmBackend::mayNeedRelaxation(const MCInst &Inst,
                                      const MCSubtargetInfo &STI) const {
  return false;
}

bool A64AsmBackend::fixupNeedsRelaxation(const MCFixup &Fixup, uint64_t Value,
                                         const MCRelaxableFragment *DF,
                                         const MCAsmLayout &Layout) const {
  return int64_t(Value) != int64_t(int8_t(Value));
}

void A64AsmBackend::relaxInstruction(MCInst &Inst,
                                     const MCSubtargetInfo &STI) const {
  llvm_unreachable("A64AsmBackend::relaxInstruction() unimplemented");
}

bool A64AsmBackend::writeNopData(raw_ostream &OS, uint64_t Count,
                                 const MCSubtargetInfo *STI) const {
  // If the count is not 4-byte aligned, we must be writing data into the text
  // section (otherwise we have unaligned instructions, and thus have far
  // bigger problems), so just write zeros instead.
  OS.write_zeros(Count % 4);

  // We are properly aligned, so write NOPs as requested.
  Count /= 4;
  for (uint64_t i = 0; i != Count; ++i)
    support::endian::write<uint32_t>(OS, 0xd503201f, support::little);
  return true;
}

namespace {

class ELFA64AsmBackend : public A64AsmBackend {
public:
  uint8_t OSABI;
  ELFA64AsmBackend(const Target &T, const Triple &TT, uint8_t OSABI)
      : A64AsmBackend(T, TT), OSABI(OSABI) {}
  std::unique_ptr<MCObjectTargetWriter>
  createObjectTargetWriter() const override {
    return createA64ELFObjectWriter(OSABI);
  }
};

} // end anonymous namespace

MCAsmBackend *llvm::createA64AsmBackend(const Target &T,
                                        const MCSubtargetInfo &STI,
                                        const MCRegisterInfo &MRI,
                                        const MCTargetOptions &Options) {
  const Triple &TheTriple = STI.getTargetTriple();
  uint8_t OSABI = MCELFObjectTargetWriter::getOSABI(TheTriple.getOS());
  return new ELFA64AsmBackend(T, TheTriple, OSABI);
}
