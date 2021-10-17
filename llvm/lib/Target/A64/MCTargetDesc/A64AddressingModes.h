//===- A64AddressingModes.h - A64 Addressing Modes ------*- C++ -*-========//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the A64 addressing mode implementation, utility functions
// for shifts, immediates etc.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_A64_MCTARGETDESC_A64ADDRESSINGMODES_H
#define LLVM_LIB_TARGET_A64_MCTARGETDESC_A64ADDRESSINGMODES_H

#include "llvm/ADT/APInt.h"
#include "llvm/ADT/bit.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/MathExtras.h"
#include <cassert>

namespace llvm {

/// A64_AM - A64 Addressing Mode Stuff
namespace A64_AM {

//===----------------------------------------------------------------------===//
// Shifts
//

enum ShiftExtendType {
  InvalidShiftExtend = -1,
  LSL = 0,
  LSR,
  ASR,
  ROR,
  MSL,

  UXTB,
  UXTH,
  UXTW,
  UXTX,

  SXTB,
  SXTH,
  SXTW,
  SXTX,
};

/// getShiftName - Get the string encoding for the shift type.
static inline const char *getShiftExtendName(A64_AM::ShiftExtendType ST) {
  switch (ST) {
  default:
    llvm_unreachable("unhandled shift type!");
  case A64_AM::LSL:
    return "lsl";
  case A64_AM::LSR:
    return "lsr";
  case A64_AM::ASR:
    return "asr";
  case A64_AM::ROR:
    return "ror";
  case A64_AM::MSL:
    return "msl";
  case A64_AM::UXTB:
    return "uxtb";
  case A64_AM::UXTH:
    return "uxth";
  case A64_AM::UXTW:
    return "uxtw";
  case A64_AM::UXTX:
    return "uxtx";
  case A64_AM::SXTB:
    return "sxtb";
  case A64_AM::SXTH:
    return "sxth";
  case A64_AM::SXTW:
    return "sxtw";
  case A64_AM::SXTX:
    return "sxtx";
  }
  return nullptr;
}

/// getShiftType - Extract the shift type.
static inline A64_AM::ShiftExtendType getShiftType(unsigned Imm) {
  switch ((Imm >> 6) & 0x7) {
  default:
    return A64_AM::InvalidShiftExtend;
  case 0:
    return A64_AM::LSL;
  case 1:
    return A64_AM::LSR;
  case 2:
    return A64_AM::ASR;
  case 3:
    return A64_AM::ROR;
  case 4:
    return A64_AM::MSL;
  }
}

/// getShiftValue - Extract the shift value.
static inline unsigned getShiftValue(unsigned Imm) { return Imm & 0x3f; }

/// getShifterImm - Encode the shift type and amount:
///   imm:     6-bit shift amount
///   shifter: 000 ==> lsl
///            001 ==> lsr
///            010 ==> asr
///            011 ==> ror
///            100 ==> msl
///   {8-6}  = shifter
///   {5-0}  = imm
static inline unsigned getShifterImm(A64_AM::ShiftExtendType ST, unsigned Imm) {
  assert((Imm & 0x3f) == Imm && "Illegal shifted immedate value!");
  unsigned STEnc = 0;
  switch (ST) {
  default:
    llvm_unreachable("Invalid shift requested");
  case A64_AM::LSL:
    STEnc = 0;
    break;
  case A64_AM::LSR:
    STEnc = 1;
    break;
  case A64_AM::ASR:
    STEnc = 2;
    break;
  case A64_AM::ROR:
    STEnc = 3;
    break;
  case A64_AM::MSL:
    STEnc = 4;
    break;
  }
  return (STEnc << 6) | (Imm & 0x3f);
}

static inline uint64_t ror(uint64_t elt, unsigned size) {
  return ((elt & 1) << (size - 1)) | (elt >> 1);
}

/// processLogicalImmediate - Determine if an immediate value can be encoded
/// as the immediate operand of a logical instruction for the given register
/// size.  If so, return true with "encoding" set to the encoded value in
/// the form N:immr:imms.
static inline bool processLogicalImmediate(uint64_t Imm, unsigned RegSize,
                                           uint64_t &Encoding) {
  if (Imm == 0ULL || Imm == ~0ULL ||
      (RegSize != 64 &&
       (Imm >> RegSize != 0 || Imm == (~0ULL >> (64 - RegSize)))))
    return false;

  // First, determine the element size.
  unsigned Size = RegSize;

  do {
    Size /= 2;
    uint64_t Mask = (1ULL << Size) - 1;

    if ((Imm & Mask) != ((Imm >> Size) & Mask)) {
      Size *= 2;
      break;
    }
  } while (Size > 2);

  // Second, determine the rotation to make the element be: 0^m 1^n.
  uint32_t CTO, I;
  uint64_t Mask = ((uint64_t)-1LL) >> (64 - Size);
  Imm &= Mask;

  if (isShiftedMask_64(Imm)) {
    I = countTrailingZeros(Imm);
    assert(I < 64 && "undefined behavior");
    CTO = countTrailingOnes(Imm >> I);
  } else {
    Imm |= ~Mask;
    if (!isShiftedMask_64(~Imm))
      return false;

    unsigned CLO = countLeadingOnes(Imm);
    I = 64 - CLO;
    CTO = CLO + countTrailingOnes(Imm) - (64 - Size);
  }

  // Encode in Immr the number of RORs it would take to get *from* 0^m 1^n
  // to our target value, where I is the number of RORs to go the opposite
  // direction.
  assert(Size > I && "I should be smaller than element size");
  unsigned Immr = (Size - I) & (Size - 1);

  // If size has a 1 in the n'th bit, create a value that has zeroes in
  // bits [0, n] and ones above that.
  uint64_t NImms = ~(Size - 1) << 1;

  // Or the CTO value into the low bits, which must be below the Nth bit
  // bit mentioned above.
  NImms |= (CTO - 1);

  // Extract the seventh bit and toggle it to create the N field.
  unsigned N = ((NImms >> 6) & 1) ^ 1;

  Encoding = (N << 12) | (Immr << 6) | (NImms & 0x3f);
  return true;
}

/// isLogicalImmediate - Return true if the immediate is valid for a logical
/// immediate instruction of the given register size. Return false otherwise.
static inline bool isLogicalImmediate(uint64_t imm, unsigned regSize) {
  uint64_t encoding;
  return processLogicalImmediate(imm, regSize, encoding);
}

/// encodeLogicalImmediate - Return the encoded immediate value for a logical
/// immediate instruction of the given register size.
static inline uint64_t encodeLogicalImmediate(uint64_t imm, unsigned regSize) {
  uint64_t encoding = 0;
  bool res = processLogicalImmediate(imm, regSize, encoding);
  assert(res && "invalid logical immediate");
  (void)res;
  return encoding;
}

/// decodeLogicalImmediate - Decode a logical immediate value in the form
/// "N:immr:imms" (where the immr and imms fields are each 6 bits) into the
/// integer value it represents with regSize bits.
static inline uint64_t decodeLogicalImmediate(uint64_t val, unsigned regSize) {
  // Extract the N, imms, and immr fields.
  unsigned N = (val >> 12) & 1;
  unsigned immr = (val >> 6) & 0x3f;
  unsigned imms = val & 0x3f;

  assert((regSize == 64 || N == 0) && "undefined logical immediate encoding");
  int len = 31 - countLeadingZeros((N << 6) | (~imms & 0x3f));
  assert(len >= 0 && "undefined logical immediate encoding");
  unsigned size = (1 << len);
  unsigned R = immr & (size - 1);
  unsigned S = imms & (size - 1);
  assert(S != size - 1 && "undefined logical immediate encoding");
  uint64_t pattern = (1ULL << (S + 1)) - 1;
  for (unsigned i = 0; i < R; ++i)
    pattern = ror(pattern, size);

  // Replicate the pattern to fill the regSize.
  while (size != regSize) {
    pattern |= (pattern << size);
    size *= 2;
  }
  return pattern;
}

/// isValidDecodeLogicalImmediate - Check to see if the logical immediate value
/// in the form "N:immr:imms" (where the immr and imms fields are each 6 bits)
/// is a valid encoding for an integer value with regSize bits.
static inline bool isValidDecodeLogicalImmediate(uint64_t val,
                                                 unsigned regSize) {
  // Extract the N and imms fields needed for checking.
  unsigned N = (val >> 12) & 1;
  unsigned imms = val & 0x3f;

  if (regSize == 32 && N != 0) // undefined logical immediate encoding
    return false;
  int len = 31 - countLeadingZeros((N << 6) | (~imms & 0x3f));
  if (len < 0) // undefined logical immediate encoding
    return false;
  unsigned size = (1 << len);
  unsigned S = imms & (size - 1);
  if (S == size - 1) // undefined logical immediate encoding
    return false;

  return true;
}

} // namespace A64_AM

} // namespace llvm
#endif
