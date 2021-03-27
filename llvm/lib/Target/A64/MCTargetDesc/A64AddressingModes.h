//===- A64AddressingModes.h - A64 Addressing Modes ------*- C++ -*-========//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the A64 addressing mode implementation stuff.
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

}

}
#endif
