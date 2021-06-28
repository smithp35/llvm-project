// RUN: llvm-mc -triple a64-elf -filetype=obj %s -o - | llvm-objdump -d -r - | FileCheck %s
// RUN: llvm-mc -triple a64-elf -show-encoding < %s | FileCheck --check-prefix CHECK-ASM %s

        .text
        ldr x0, [x1]
        ldr x1, [x2, #0x8]
        .global external
        ldr x0, [x1, :lo12: external]
        ldr x0, [x2, :lo12: local]
local:

// CHECK: 0000000000000000 <.text>:
// CHECK-NEXT: 0: f9400020   ldr     x0, [x1]
// CHECK-NEXT: 4: f9400441   ldr     x1, [x2, #8]
// CHECK-NEXT: 8: f9400020   ldr     x0, [x1]
// CHECK-NEXT: 0000000000000008:  R_AARCH64_LDST64_ABS_LO12_NC external
// CHECK-NEXT: c: f9400040   ldr     x0, [x2]
// CHECK-NEXT: 000000000000000c:  R_AARCH64_LDST64_ABS_LO12_NC .text+0x10

// CHECK-ASM:      ldr     x0, [x1]     // encoding: [0x20,0x00,0x40,0xf9]
// CHECK-ASM-NEXT: ldr     x1, [x2, #8] // encoding: [0x41,0x04,0x40,0xf9]
// CHECK-ASM:      ldr     x0, [x1, :lo12:external]        // encoding: [0x20,0bAAAAAA00,0b01AAAAAA,0xf9]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: :lo12:external, kind: fixup_a64_ldst_imm12_scale8
// CHECK-ASM-NEXT: ldr     x0, [x2, :lo12:local]           // encoding: [0x40,0bAAAAAA00,0b01AAAAAA,0xf9]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: :lo12:local, kind: fixup_a64_ldst_imm12_scale8
