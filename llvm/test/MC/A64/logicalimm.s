// RUN: llvm-mc -triple a64-elf -filetype=obj %s -o - | llvm-objdump -d -r - | FileCheck %s
// RUN: llvm-mc -triple a64-elf -show-encoding < %s | FileCheck -check-prefix=CHECK-ASM %s

        .text
        and x0, x0, #1
        bic x11, x12, #2
        orr x1, x2, #4
        orn x5, x6, #8
        eor x3, x4, #16
        eon x7, x8, #32

/// flag setting
        ands x0, x0, #64
        bics x2, x4, #128

/// large logical shifts
        and x0, x0, #0xf0000000
        orr x1, x2, #0x10000000

// CHECK: 0000000000000000 <.text>:
// CHECK-NEXT:  0: 92400000    and     x0, x0, #0x1
// CHECK-NEXT:  4: 927ef98b    and     x11, x12, #0xfffffffffffffffd
// CHECK-NEXT:  8: b27e0041    orr     x1, x2, #0x4
// CHECK-NEXT:  c: b27cf8c5    orr     x5, x6, #0xfffffffffffffff7
// CHECK-NEXT: 10: d27c0083    eor     x3, x4, #0x10
// CHECK-NEXT: 14: d27af907    eor     x7, x8, #0xffffffffffffffdf
// CHECK-NEXT: 18: f27a0000    ands    x0, x0, #0x40
// CHECK-NEXT: 1c: f278f882    ands    x2, x4, #0xffffffffffffff7f
// CHECK-NEXT: 20: 92640c00    and     x0, x0, #0xf0000000
// CHECK-NEXT: 24: b2640041    orr     x1, x2, #0x10000000

// CHECK-ASM:      and x0, x0, #0x1 // encoding: [0x00,0x00,0x40,0x92]
// CHECK-ASM-NEXT: and x11, x12, #0xfffffffffffffffd // encoding: [0x8b,0xf9,0x7e,0x92]
// CHECK-ASM-NEXT: orr x1, x2, #0x4 // encoding: [0x41,0x00,0x7e,0xb2]
// CHECK-ASM-NEXT: orr x5, x6, #0xfffffffffffffff7 // encoding: [0xc5,0xf8,0x7c,0xb2]
// CHECK-ASM-NEXT: eor x3, x4, #0x10 // encoding: [0x83,0x00,0x7c,0xd2]
// CHECK-ASM-NEXT: eor x7, x8, #0xffffffffffffffdf // encoding: [0x07,0xf9,0x7a,0xd2]
// CHECK-ASM:      ands x0, x0, #0x40 // encoding: [0x00,0x00,0x7a,0xf2]
// CHECK-ASM-NEXT: ands x2, x4, #0xffffffffffffff7f        // encoding: [0x82,0xf8,0x78,0xf2]
// CHECK-ASM:      and x0, x0, #0xf0000000                 // encoding: [0x00,0x0c,0x64,0x92]
// CHECK-ASM-NEXT: orr x1, x2, #0x10000000                 // encoding: [0x41,0x00,0x64,0xb2]
