// RUN: llvm-mc -triple a64-elf -filetype=obj %s -o - | llvm-objdump -d -r - | FileCheck %s
// RUN: llvm-mc -triple a64-elf -show-encoding < %s | FileCheck --check-prefix CHECK-ASM %s
        .text
        .global global
        // Unsigned relocations
        movn x0, :abs_g3: global
        movn x0, :abs_g2: global
        movn x0, :abs_g2_nc: global
        movn x0, :abs_g1: global
        movn x0, :abs_g1_nc: global
        movn x0, :abs_g0: global
        movn x0, :abs_g0_nc: global

        movz x0, :abs_g3: global
        movz x0, :abs_g2: global
        movz x0, :abs_g2_nc: global
        movz x0, :abs_g1: global
        movz x0, :abs_g1_nc: global
        movz x0, :abs_g0: global
        movz x0, :abs_g0_nc: global

        movk x0, :abs_g3: global
        movk x0, :abs_g2: global
        movk x0, :abs_g2_nc: global
        movk x0, :abs_g1: global
        movk x0, :abs_g1_nc: global
        movk x0, :abs_g0: global
        movk x0, :abs_g0_nc: global

        // Signed relocations
        movn x0, :abs_g2_s: global
        movn x0, :abs_g1_s: global
        movn x0, :abs_g0_s: global

        movz x0, :abs_g2_s: global
        movz x0, :abs_g1_s: global
        movz x0, :abs_g0_s: global

        movk x0, :abs_g2_s: global
        movk x0, :abs_g1_s: global
        movk x0, :abs_g0_s: global

// CHECK: 0000000000000000 <.text>:
// CHECK-NEXT: 0: 92e00000    movn    x0, #0, lsl #48
// CHECK-NEXT: 0000000000000000:  R_AARCH64_MOVW_UABS_G3       global
// CHECK-NEXT: 4: 92c00000    movn    x0, #0, lsl #32
// CHECK-NEXT: 0000000000000004:  R_AARCH64_MOVW_UABS_G2       global
// CHECK-NEXT: 8: 92c00000    movn    x0, #0, lsl #32
// CHECK-NEXT: 0000000000000008:  R_AARCH64_MOVW_UABS_G2_NC    global
// CHECK-NEXT: c: 92a00000    movn    x0, #0, lsl #16
// CHECK-NEXT: 000000000000000c:  R_AARCH64_MOVW_UABS_G1       global
// CHECK-NEXT:10: 92a00000    movn    x0, #0, lsl #16
// CHECK-NEXT: 0000000000000010:  R_AARCH64_MOVW_UABS_G1_NC    global
// CHECK-NEXT:14: 92800000    mov     x0, #-1
// CHECK-NEXT: 0000000000000014:  R_AARCH64_MOVW_UABS_G0       global
// CHECK-NEXT:18: 92800000    mov     x0, #-1
// CHECK-NEXT: 0000000000000018:  R_AARCH64_MOVW_UABS_G0_NC    global
// CHECK-NEXT:1c: d2e00000    movz    x0, #0, lsl #48
// CHECK-NEXT: 000000000000001c:  R_AARCH64_MOVW_UABS_G3       global
// CHECK-NEXT:20: d2c00000    movz    x0, #0, lsl #32
// CHECK-NEXT: 0000000000000020:  R_AARCH64_MOVW_UABS_G2       global
// CHECK-NEXT:24: d2c00000    movz    x0, #0, lsl #32
// CHECK-NEXT: 0000000000000024:  R_AARCH64_MOVW_UABS_G2_NC    global
// CHECK-NEXT:28: d2a00000    movz    x0, #0, lsl #16
// CHECK-NEXT: 0000000000000028:  R_AARCH64_MOVW_UABS_G1       global
// CHECK-NEXT:2c: d2a00000    movz    x0, #0, lsl #16
// CHECK-NEXT: 000000000000002c:  R_AARCH64_MOVW_UABS_G1_NC    global
// CHECK-NEXT:30: d2800000    mov     x0, #0
// CHECK-NEXT: 0000000000000030:  R_AARCH64_MOVW_UABS_G0       global
// CHECK-NEXT:34: d2800000    mov     x0, #0
// CHECK-NEXT: 0000000000000034:  R_AARCH64_MOVW_UABS_G0_NC    global
// CHECK-NEXT:38: f2e00000    movk    x0, #0, lsl #48
// CHECK-NEXT: 0000000000000038:  R_AARCH64_MOVW_UABS_G3       global
// CHECK-NEXT:3c: f2c00000    movk    x0, #0, lsl #32
// CHECK-NEXT: 000000000000003c:  R_AARCH64_MOVW_UABS_G2       global
// CHECK-NEXT:40: f2c00000    movk    x0, #0, lsl #32
// CHECK-NEXT: 0000000000000040:  R_AARCH64_MOVW_UABS_G2_NC    global
// CHECK-NEXT:44: f2a00000    movk    x0, #0, lsl #16
// CHECK-NEXT: 0000000000000044:  R_AARCH64_MOVW_UABS_G1       global
// CHECK-NEXT:48: f2a00000    movk    x0, #0, lsl #16
// CHECK-NEXT: 0000000000000048:  R_AARCH64_MOVW_UABS_G1_NC    global
// CHECK-NEXT:4c: f2800000    movk    x0, #0
// CHECK-NEXT: 000000000000004c:  R_AARCH64_MOVW_UABS_G0       global
// CHECK-NEXT:50: f2800000    movk    x0, #0
// CHECK-NEXT: 0000000000000050:  R_AARCH64_MOVW_UABS_G0_NC    global
// CHECK-NEXT: 54: 92c00000   movn    x0, #0, lsl #32
// CHECK-NEXT: 0000000000000054:  R_AARCH64_MOVW_SABS_G2       global
// CHECK-NEXT: 58: 92a00000   movn    x0, #0, lsl #16
// CHECK-NEXT: 0000000000000058:  R_AARCH64_MOVW_SABS_G1       global
// CHECK-NEXT: 5c: 92800000   mov     x0, #-1
// CHECK-NEXT: 000000000000005c:  R_AARCH64_MOVW_SABS_G0       global
// CHECK-NEXT: 60: d2c00000   movz    x0, #0, lsl #32
// CHECK-NEXT: 0000000000000060:  R_AARCH64_MOVW_SABS_G2       global
// CHECK-NEXT: 64: d2a00000   movz    x0, #0, lsl #16
// CHECK-NEXT: 0000000000000064:  R_AARCH64_MOVW_SABS_G1       global
// CHECK-NEXT: 68: d2800000   mov     x0, #0
// CHECK-NEXT: 0000000000000068:  R_AARCH64_MOVW_SABS_G0       global
// CHECK-NEXT: 6c: f2c00000   movk    x0, #0, lsl #32
// CHECK-NEXT: 000000000000006c:  R_AARCH64_MOVW_SABS_G2       global
// CHECK-NEXT: 70: f2a00000   movk    x0, #0, lsl #16
// CHECK-NEXT: 0000000000000070:  R_AARCH64_MOVW_SABS_G1       global
// CHECK-NEXT: 74: f2800000   movk    x0, #0
// CHECK-NEXT: 0000000000000074:  R_AARCH64_MOVW_SABS_G0       global

// CHECK-ASM:         .globl  global
// CHECK-ASM-NEXT: movn    x0, #:abs_g3:global             // encoding: [0bAAA00000,A,0b111AAAAA,0x92]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: :abs_g3:global, kind: fixup_a64_movw
// CHECK-ASM-NEXT: movn    x0, #:abs_g2:global             // encoding: [0bAAA00000,A,0b110AAAAA,0x92]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: :abs_g2:global, kind: fixup_a64_movw
// CHECK-ASM-NEXT: movn    x0, #:abs_g2_nc:global          // encoding: [0bAAA00000,A,0b110AAAAA,0x92]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: :abs_g2_nc:global, kind: fixup_a64_movw
// CHECK-ASM-NEXT: movn    x0, #:abs_g1:global             // encoding: [0bAAA00000,A,0b101AAAAA,0x92]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: :abs_g1:global, kind: fixup_a64_movw
// CHECK-ASM-NEXT: movn    x0, #:abs_g1_nc:global          // encoding: [0bAAA00000,A,0b101AAAAA,0x92]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: :abs_g1_nc:global, kind: fixup_a64_movw
// CHECK-ASM-NEXT: movn    x0, #:abs_g0:global             // encoding: [0bAAA00000,A,0b100AAAAA,0x92]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: :abs_g0:global, kind: fixup_a64_movw
// CHECK-ASM-NEXT: movn    x0, #:abs_g0_nc:global          // encoding: [0bAAA00000,A,0b100AAAAA,0x92]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: :abs_g0_nc:global, kind: fixup_a64_movw
// CHECK-ASM-EMPTY:
// CHECK-ASM-NEXT: movz    x0, #:abs_g3:global             // encoding: [0bAAA00000,A,0b111AAAAA,0xd2]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: :abs_g3:global, kind: fixup_a64_movw
// CHECK-ASM-NEXT: movz    x0, #:abs_g2:global             // encoding: [0bAAA00000,A,0b110AAAAA,0xd2]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: :abs_g2:global, kind: fixup_a64_movw
// CHECK-ASM-NEXT: movz    x0, #:abs_g2_nc:global          // encoding: [0bAAA00000,A,0b110AAAAA,0xd2]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: :abs_g2_nc:global, kind: fixup_a64_movw
// CHECK-ASM-NEXT: movz    x0, #:abs_g1:global             // encoding: [0bAAA00000,A,0b101AAAAA,0xd2]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: :abs_g1:global, kind: fixup_a64_movw
// CHECK-ASM-NEXT: movz    x0, #:abs_g1_nc:global          // encoding: [0bAAA00000,A,0b101AAAAA,0xd2]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: :abs_g1_nc:global, kind: fixup_a64_movw
// CHECK-ASM-NEXT: movz    x0, #:abs_g0:global             // encoding: [0bAAA00000,A,0b100AAAAA,0xd2]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: :abs_g0:global, kind: fixup_a64_movw
// CHECK-ASM-NEXT: movz    x0, #:abs_g0_nc:global          // encoding: [0bAAA00000,A,0b100AAAAA,0xd2]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: :abs_g0_nc:global, kind: fixup_a64_movw
// CHECK-ASM-EMPTY:
// CHECK-ASM-NEXT: movk    x0, :abs_g3:global              // encoding: [0bAAA00000,A,0b111AAAAA,0xf2]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: :abs_g3:global, kind: fixup_a64_movw
// CHECK-ASM-NEXT: movk    x0, :abs_g2:global              // encoding: [0bAAA00000,A,0b110AAAAA,0xf2]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: :abs_g2:global, kind: fixup_a64_movw
// CHECK-ASM-NEXT: movk    x0, :abs_g2_nc:global           // encoding: [0bAAA00000,A,0b110AAAAA,0xf2]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: :abs_g2_nc:global, kind: fixup_a64_movw
// CHECK-ASM-NEXT: movk    x0, :abs_g1:global              // encoding: [0bAAA00000,A,0b101AAAAA,0xf2]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: :abs_g1:global, kind: fixup_a64_movw
// CHECK-ASM-NEXT: movk    x0, :abs_g1_nc:global           // encoding: [0bAAA00000,A,0b101AAAAA,0xf2]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: :abs_g1_nc:global, kind: fixup_a64_movw
// CHECK-ASM-NEXT: movk    x0, :abs_g0:global              // encoding: [0bAAA00000,A,0b100AAAAA,0xf2]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: :abs_g0:global, kind: fixup_a64_movw
// CHECK-ASM-NEXT: movk    x0, :abs_g0_nc:global           // encoding: [0bAAA00000,A,0b100AAAAA,0xf2]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: :abs_g0_nc:global, kind: fixup_a64_movw
// CHECK-ASM-EMPTY:
// CHECK-ASM-NEXT: movn    x0, #:abs_g2_s:global           // encoding: [0bAAA00000,A,0b110AAAAA,0x92]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: :abs_g2_s:global, kind: fixup_a64_movw
// CHECK-ASM-NEXT: movn    x0, #:abs_g1_s:global           // encoding: [0bAAA00000,A,0b101AAAAA,0x92]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: :abs_g1_s:global, kind: fixup_a64_movw
// CHECK-ASM-NEXT: movn    x0, #:abs_g0_s:global           // encoding: [0bAAA00000,A,0b100AAAAA,0x92]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: :abs_g0_s:global, kind: fixup_a64_movw
// CHECK-ASM-EMPTY: 
// CHECK-ASM-NEXT: movz    x0, #:abs_g2_s:global           // encoding: [0bAAA00000,A,0b110AAAAA,0xd2]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: :abs_g2_s:global, kind: fixup_a64_movw
// CHECK-ASM-NEXT: movz    x0, #:abs_g1_s:global           // encoding: [0bAAA00000,A,0b101AAAAA,0xd2]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: :abs_g1_s:global, kind: fixup_a64_movw
// CHECK-ASM-NEXT: movz    x0, #:abs_g0_s:global           // encoding: [0bAAA00000,A,0b100AAAAA,0xd2]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: :abs_g0_s:global, kind: fixup_a64_movw
// CHECK-ASM-EMPTY: 
// CHECK-ASM-NEXT: movk    x0, :abs_g2_s:global            // encoding: [0bAAA00000,A,0b110AAAAA,0xf2]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: :abs_g2_s:global, kind: fixup_a64_movw
// CHECK-ASM-NEXT: movk    x0, :abs_g1_s:global            // encoding: [0bAAA00000,A,0b101AAAAA,0xf2]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: :abs_g1_s:global, kind: fixup_a64_movw
// CHECK-ASM-NEXT: movk    x0, :abs_g0_s:global            // encoding: [0bAAA00000,A,0b100AAAAA,0xf2]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: :abs_g0_s:global, kind: fixup_a64_movw

