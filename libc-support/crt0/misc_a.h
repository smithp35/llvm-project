//
// Copyright (c) 2025, Arm Limited and affiliates.
//
// Part of the Arm Toolchain project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//

// A-profile miscellaneous setup

#ifndef BOOTCODE_MISC_A_H
#define BOOTCODE_MISC_A_H

#include "system_registers_a.h"

namespace bootcode {
namespace misc {

using namespace sysreg;

#ifdef __ARM_FEATURE_PAUTH
extern "C" {
extern uintptr_t __rela_dyn_start;
extern uintptr_t __rela_dyn_end;
}

typedef struct {
  uint64_t r_offset;
  uint64_t r_info;
  int64_t r_addend;
} Relocation;

#define ELF64_R_SYM(i) ((i) >> 32)
#define ELF64_R_TYPE(i) ((i) & 0xffffffffL)
#define R_AARCH64_AUTH_RELATIVE 0x411

#define APIA 0b00
#define APIB 0b01
#define APDA 0b10
#define APDB 0b11

void init_global_pointers(void) {
  Relocation *rstart = (Relocation *)(uintptr_t)&__rela_dyn_start;
  Relocation *rend = (Relocation *)(uintptr_t)&__rela_dyn_end;
  for (Relocation *rp = rstart; rp != rend; ++rp) {
    uint64_t address = rp->r_addend;
    switch (ELF64_R_TYPE(rp->r_info)) {
    case R_AARCH64_AUTH_RELATIVE: {
      uint64_t schema = *(uint64_t *)rp->r_offset;
      uint64_t address_diversity = schema >> 63;
      uint64_t key = (schema >> 60) & 0x3;
      uint64_t discriminator = (schema >> 32) & 0xffff;
      uint64_t modifier = 0x0;
      if (address_diversity) {
        if (discriminator)
          modifier =
              (rp->r_offset & 0x0000ffffffffffff) | (discriminator << 48);
        else
          modifier = rp->r_offset;
      } else
        modifier = discriminator;

      if (key == APIA)
        __asm__ __volatile__("pacia %0, %1" : "+r"(address) : "r"(modifier));
      else if (key == APIB)
        __asm__ __volatile__("pacib %0, %1" : "+r"(address) : "r"(modifier));
      else if (key == APDA)
        __asm__ __volatile__("pacda %0, %1" : "+r"(address) : "r"(modifier));
      else
        __asm__ __volatile__("pacdb %0, %1" : "+r"(address) : "r"(modifier));
      *(uint64_t *)rp->r_offset = address;
      break;
    }
    default:
      // Should abort here for unhandled relocation. Expect only
      // R_AARCH64_AUTH_RELATIVE in bare-metal projects.
      break;
    }
  }
}

#endif // __ARM_FEATURE_PAUTH

void setup() {
#ifdef __ARM_FEATURE_PAUTH
  // Set all of the pointer authentication keys to different values. In
  // reality these would be set to random values to prevent attackers from
  // being able to predict signed pointer values, but we just set them to
  // arbitary, fixed values for testing.
  APIAKeyLo = 0x1111111111111111;
  APIAKeyHi = 0x1111111111111111;
  APIBKeyLo = 0x2222222222222222;
  APIBKeyHi = 0x2222222222222222;
  APDAKeyLo = 0x3333333333333333;
  APDAKeyHi = 0x3333333333333333;
  APDBKeyLo = 0x4444444444444444;
  APDBKeyHi = 0x4444444444444444;
  APGAKeyLo = 0x5555555555555555;
  APGAKeyHi = 0x5555555555555555;
  // Enable all pointer authentication instructions at the current EL. There
  // is one bit of SCTLR for each of the IA, IB, DA and DB keys. There is no
  // bit for the GA key, which is always enabled.
  SCTLR.EnIA = 1;
  SCTLR.EnIB = 1;
  SCTLR.EnDB = 1;
  SCTLR.EnDA = 1;
#endif

#ifdef __ARM_FP
  // CPACR enable access to vfp and simd
  CPACR.CP10 = 0x3;
  CPACR.CP11 = 0x3;
#if defined(__ARM_ARCH_ISA_A64)
  // Don't trap FP instructions
  CPTR.TFP = 0;
  // Don't trap SVE or SME instructions, for targets which do not have the
  // relevant feature these bits are ignored, so safe to set unconditionally.
  CPTR.EZ = 1;
  CPTR.ESM = 1;
#else
  // Enable VFP and SIMD
  __arm_wsr("fpexc", 1 << 30);
#endif
#endif

  // Enable cycle counter in EL2 if PMUv3 is implementerd
  if (ID_DFR0.PerfMon != 0xF && ID_DFR0.PerfMon >= 3)
    PMCCFILTR.NSH = 1;

#if !defined(__ARM_ARCH_ISA_A64) &&                                            \
    !(__ARM_ARCH_PROFILE == 'R' && __ARM_ARCH >= 8)
  // Copy the current sp value to each of the banked copies of sp.
  __arm_wsr("CPSR_c", 0x11); // FIQ
  asm volatile("mov sp, %0" : : "r"(__builtin_frame_address(0)));
  __arm_wsr("CPSR_c", 0x12); // IRQ
  asm volatile("mov sp, %0" : : "r"(__builtin_frame_address(0)));
  __arm_wsr("CPSR_c", 0x17); // ABT
  asm volatile("mov sp, %0" : : "r"(__builtin_frame_address(0)));
  __arm_wsr("CPSR_c", 0x1B); // UND
  asm volatile("mov sp, %0" : : "r"(__builtin_frame_address(0)));
  __arm_wsr("CPSR_c", 0x1F); // SYS
  asm volatile("mov sp, %0" : : "r"(__builtin_frame_address(0)));
  __arm_wsr("CPSR_c", 0x13); // SVC
#endif
}

} // namespace misc
} // namespace bootcode

#endif // BOOTCODE_MISC_A_H
