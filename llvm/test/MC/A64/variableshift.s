// RUN: llvm-mc -triple a64-elf -filetype=obj %s -o - | llvm-objdump -d -r - | FileCheck %s
// RUN: llvm-mc -triple a64-elf -show-encoding < %s | FileCheck -check-prefix=CHECK-ASM %s

        .text
        asr x0, x1, x2
        lsl x2, x3, x4
        lsr x4, x5, x5
        ror x6, x7, x8

// CHECK:      0: 9ac22820   asr     x0, x1, x2
// CHECK-NEXT: 4: 9ac42062   lsl     x2, x3, x4
// CHECK-NEXT: 8: 9ac524a4   lsr     x4, x5, x5
// CHECK-NEXT: c: 9ac82ce6   ror     x6, x7, x8

// CHECK-ASM:      asr  x0, x1, x2 // encoding: [0x20,0x28,0xc2,0x9a]
// CHECK-ASM-NEXT: lsl  x2, x3, x4 // encoding: [0x62,0x20,0xc4,0x9a]
// CHECK-ASM-NEXT: lsr  x4, x5, x5 // encoding: [0xa4,0x24,0xc5,0x9a]
// CHECK-ASM-NEXT: ror  x6, x7, x8 // encoding: [0xe6,0x2c,0xc8,0x9a]
