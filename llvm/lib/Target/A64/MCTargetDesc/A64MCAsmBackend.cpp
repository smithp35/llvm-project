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
#include "llvm/MC/MCFixupKindInfo.h"
#include "llvm/Support/EndianStream.h"

using namespace llvm;

namespace {

class A64AsmBackend : public MCAsmBackend {
  static const unsigned PCRelFlagVal =
      MCFixupKindInfo::FKF_IsAlignedDownTo32Bits | MCFixupKindInfo::FKF_IsPCRel;

protected:
  Triple TheTriple;

public:
  A64AsmBackend(const Target &T, const Triple &TT)
      : MCAsmBackend(support::little), TheTriple(TT) {}

  unsigned getPointerSize() const { return 8; }

  unsigned getNumFixupKinds() const override {
    return A64::NumTargetFixupKinds;
  }

  const MCFixupKindInfo &getFixupKindInfo(MCFixupKind Kind) const override {
    const static MCFixupKindInfo Infos[A64::NumTargetFixupKinds] = {
        // This table *must* be in the order that the fixup_* kinds are defined
        // in A64FixupKinds.h.
        //
        // Name                           Offset (bits) Size (bits)     Flags
        {"fixup_64_pcrel_adr_imm21", 0, 32, PCRelFlagVal},
        {"fixup_a64_add_imm12", 10, 12, 0},
        {"fixup_a64_pcrel_branch19", 5, 19, PCRelFlagVal},
        {"fixup_a64_pcrel_branch26", 0, 26, PCRelFlagVal},
        {"fixup_a64_pcrel_call26", 0, 26, PCRelFlagVal}};

    // Fixup kinds from .reloc directive are like R_A64_NONE. They do not
    // require any extra processing.
    if (Kind >= FirstLiteralRelocationKind)
      return MCAsmBackend::getFixupKindInfo(FK_NONE);

    if (Kind < FirstTargetFixupKind)
      return MCAsmBackend::getFixupKindInfo(Kind);

    assert(unsigned(Kind - FirstTargetFixupKind) < getNumFixupKinds() &&
           "Invalid kind!");
    return Infos[Kind - FirstTargetFixupKind];
  }

  void applyFixup(const MCAssembler &Asm, const MCFixup &Fixup,
                  const MCValue &Target, MutableArrayRef<char> Data,
                  uint64_t Value, bool IsResolved,
                  const MCSubtargetInfo *STI) const override;

  bool fixupNeedsRelaxation(const MCFixup &Fixup, uint64_t Value,
                            const MCRelaxableFragment *DF,
                            const MCAsmLayout &Layout) const override;
  void relaxInstruction(MCInst &Inst,
                        const MCSubtargetInfo &STI) const override;

  bool writeNopData(raw_ostream &OS, uint64_t Count,
                    const MCSubtargetInfo *STI) const override;
};

} // end anonymous namespace

/// The number of bytes the fixup may change.
static unsigned getFixupKindNumBytes(unsigned Kind) {
  switch (Kind) {
  default:
    llvm_unreachable("Unknown fixup kind!");

  case FK_Data_1:
    return 1;

  case FK_Data_2:
  case FK_SecRel_2:
    return 2;

  case A64::fixup_a64_add_imm12:
  case A64::fixup_a64_pcrel_branch19:
    return 3;

  case A64::fixup_a64_pcrel_adr_imm21:
  case A64::fixup_a64_pcrel_branch26:
  case A64::fixup_a64_pcrel_call26:
  case FK_Data_4:
  case FK_SecRel_4:
    return 4;

  case FK_Data_8:
    return 8;
  }
}

static unsigned AdrImmBits(unsigned Value) {
  unsigned lo2 = Value & 0x3;
  unsigned hi19 = (Value & 0x1ffffc) >> 2;
  return (hi19 << 5) | (lo2 << 29);
}

static uint64_t adjustFixupValue(const MCFixup &Fixup, const MCValue &Target,
                                 uint64_t Value, MCContext &Ctx,
                                 const Triple &TheTriple, bool IsResolved) {
  int64_t SignedValue = static_cast<int64_t>(Value);
  switch (Fixup.getTargetKind()) {
  default:
    llvm_unreachable("Unknown fixup kind!");
  case A64::fixup_a64_pcrel_adr_imm21:
    if (SignedValue > 2097151 || SignedValue < -2097152)
      Ctx.reportError(Fixup.getLoc(), "fixup value out of range");
    return AdrImmBits(Value & 0x1fffffULL);
  case A64::fixup_a64_add_imm12:
    if (Value >= 0x1000)
      Ctx.reportError(Fixup.getLoc(), "fixup value out of range");
    return Value;
  case A64::fixup_a64_pcrel_branch19:
    // Signed 21-bit immediate
    if (SignedValue > 2097151 || SignedValue < -2097152)
      Ctx.reportError(Fixup.getLoc(), "fixup value out of range");
    if (Value & 0x3)
      Ctx.reportError(Fixup.getLoc(), "fixup not sufficiently aligned");
    // Low two bits are not encoded.
    return (Value >> 2) & 0x7ffff;
  case A64::fixup_a64_pcrel_branch26:
  case A64::fixup_a64_pcrel_call26:
    // Signed 28-bit immediate
    if (SignedValue > 134217727 || SignedValue < -134217728)
      Ctx.reportError(Fixup.getLoc(), "fixup value out of range");
    // Low two bits are not encoded (4-byte alignment assumed).
    if (Value & 0x3)
      Ctx.reportError(Fixup.getLoc(), "fixup not sufficiently aligned");
    return (Value >> 2) & 0x3ffffff;
  case FK_Data_1:
  case FK_Data_2:
  case FK_Data_4:
  case FK_Data_8:
  case FK_SecRel_2:
  case FK_SecRel_4:
    return Value;
  }
}

void A64AsmBackend::applyFixup(const MCAssembler &Asm, const MCFixup &Fixup,
                               const MCValue &Target,
                               MutableArrayRef<char> Data, uint64_t Value,
                               bool IsResolved,
                               const MCSubtargetInfo *STI) const {
  if (!Value)
    return; // Doesn't change encoding.
  unsigned Kind = Fixup.getKind();
  if (Kind >= FirstLiteralRelocationKind)
    return;
  unsigned NumBytes = getFixupKindNumBytes(Kind);
  MCFixupKindInfo Info = getFixupKindInfo(Fixup.getKind());
  MCContext &Ctx = Asm.getContext();
  // Apply any target-specific value adjustments.
  Value = adjustFixupValue(Fixup, Target, Value, Ctx, TheTriple, IsResolved);

  // Shift the value into position.
  Value <<= Info.TargetOffset;

  unsigned Offset = Fixup.getOffset();
  assert(Offset + NumBytes <= Data.size() && "Invalid fixup offset!");

  // Little endian only
  for (unsigned i = 0; i != NumBytes; ++i)
    Data[Offset + i] |= uint8_t((Value >> (i * 8)) & 0xff);
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
