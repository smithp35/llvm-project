// RUN: llvm-mc -triple a64-elf -filetype=obj %s -o - | llvm-objdump -d -r - | FileCheck %s
// RUN: llvm-mc -triple a64-elf -show-encoding < %s | FileCheck -check-prefix=CHECK-ASM %s

        .text
/// Immediate form
        add x0, x0, #0
        add x1, x2, #1

/// Register form
        add x0, x1, x2

/// Flag setting variant
        adds x0, x0, #0
        adds x0, x1, x2

// Shifted register form
        add x3, x4, x5, lsl 1
        add x8, x20, x21, lsr 33
        add x9, x30, x29, asr 63

        adds x3, x4, x5, lsl 1
        adds x8, x20, x21, lsr 33
        adds x9, x30, x29, asr 63
// CHECK:       0: 91000000 add     x0, x0, #0
// CHECK-NEXT:  4: 91000441 add     x1, x2, #1
// CHECK-NEXT:  8: 8b020020 add     x0, x1, x2
// CHECK-NEXT:  c: b1000000 adds    x0, x0, #0
// CHECK-NEXT: 10: ab020020 adds    x0, x1, x2
// CHECK-NEXT: 14: 8b050483 add     x3, x4, x5, lsl #1
// CHECK-NEXT: 18: 8b558688 add     x8, x20, x21, lsr #33
// CHECK-NEXT: 1c: 8b9dffc9 add     x9, x30, x29, asr #63
// CHECK-NEXT: 20: ab050483 adds    x3, x4, x5, lsl #1
// CHECK-NEXT: 24: ab558688 adds    x8, x20, x21, lsr #33
// CHECK-NEXT: 28: ab9dffc9 adds    x9, x30, x29, asr #63

// CHECK-ASM:      add  x0, x0, #0 // encoding: [0x00,0x00,0x00,0x91]
// CHECK-ASM-NEXT: add  x1, x2, #1 // encoding: [0x41,0x04,0x00,0x91]

// CHECK-ASM:      add  x0, x1, x2 // encoding: [0x20,0x00,0x02,0x8b]

// CHECK-ASM:      adds x0, x0, #0 // encoding: [0x00,0x00,0x00,0xb1]
// CHECK-ASM-NEXT: adds x0, x1, x2 // encoding: [0x20,0x00,0x02,0xab]
// CHECK-ASM:      add x3, x4, x5, lsl #1   // encoding: [0x83,0x04,0x05,0x8b]
// CHECK-ASM-NEXT: add x8, x20, x21, lsr #33 // encoding: [0x88,0x86,0x55,0x8b]
// CHECK-ASM-NEXT: add x9, x30, x29, asr #63 // encoding: [0xc9,0xff,0x9d,0x8b]
// CHECK-ASM:      adds x3, x4, x5, lsl #1  // encoding: [0x83,0x04,0x05,0xab]
// CHECK-ASM-NEXT: adds x8, x20, x21, lsr #33 // encoding: [0x88,0x86,0x55,0xab]
// CHECK-ASM-NEXT: adds x9, x30, x29, asr #63 // encoding: [0xc9,0xff,0x9d,0xab]
