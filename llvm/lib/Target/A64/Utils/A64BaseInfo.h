//===-- A64BaseInfo.h - Top level definitions for A64 ---*- C++ -*-==========//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains small standalone helper functions and enum definitions for
// the A64 target useful for the compiler back-end and the MC libraries.
// As such, it deliberately does not include references to LLVM core
// code gen types, passes, etc..
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AARCH64_UTILS_A64BASEINFO_H
#define LLVM_LIB_TARGET_AARCH64_UTILS_A64BASEINFO_H

#include "llvm/Support/ErrorHandling.h"

namespace A64II {
  /// Target Operand Flag enum.
  enum TOF {
    //===------------------------------------------------------------------===//
    // A64 Specific MachineOperand flags.
    MO_NO_FLAG,

    MO_FRAGMENT = 0x7,

    /// MO_PAGE - A symbol operand with this flag represents the pc-relative
    /// offset of the 4K page containing the symbol.  This is used with the
    /// ADRP instruction.
    MO_PAGE = 1,

    /// MO_PAGEOFF - A symbol operand with this flag represents the offset of
    /// that symbol within a 4K page.  This offset is added to the page address
    /// to produce the complete address.
    MO_PAGEOFF = 2,

    /// MO_NC - Indicates whether the linker is expected to check the symbol
    /// reference for overflow. For example in an ADRP/ADD pair of relocations
    /// the ADRP usually does check, but not the ADD.
    MO_NC = 0x20,

  };
}

namespace A64CC {



// The CondCodes constants map directly to the 4-bit encoding of the condition
// field for predicated instructions.
enum CondCode { // Meaning (integer)          Meaning (floating-point)
  EQ = 0x0,     // Equal                      Equal
  NE = 0x1,     // Not equal                  Not equal, or unordered
  HS = 0x2,     // Unsigned higher or same    >, ==, or unordered
  LO = 0x3,     // Unsigned lower             Less than
  MI = 0x4,     // Minus, negative            Less than
  PL = 0x5,     // Plus, positive or zero     >, ==, or unordered
  VS = 0x6,     // Overflow                   Unordered
  VC = 0x7,     // No overflow                Not unordered
  HI = 0x8,     // Unsigned higher            Greater than, or unordered
  LS = 0x9,     // Unsigned lower or same     Less than or equal
  GE = 0xa,     // Greater than or equal      Greater than or equal
  LT = 0xb,     // Less than                  Less than, or unordered
  GT = 0xc,     // Greater than               Greater than
  LE = 0xd,     // Less than or equal         <, ==, or unordered
  AL = 0xe,     // Always (unconditional)     Always (unconditional)
  NV = 0xf,     // Always (unconditional)     Always (unconditional)
  // Note the NV exists purely to disassemble 0b1111. Execution is "always".
  Invalid,
};

inline static const char *getCondCodeName(CondCode Code) {
  switch (Code) {
  default:
    llvm_unreachable("Unknown condition code");
  case EQ:
    return "eq";
  case NE:
    return "ne";
  case HS:
    return "hs";
  case LO:
    return "lo";
  case MI:
    return "mi";
  case PL:
    return "pl";
  case VS:
    return "vs";
  case VC:
    return "vc";
  case HI:
    return "hi";
  case LS:
    return "ls";
  case GE:
    return "ge";
  case LT:
    return "lt";
  case GT:
    return "gt";
  case LE:
    return "le";
  case AL:
    return "al";
  case NV:
    return "nv";
  }
}
} // namespace A64CC
#endif
