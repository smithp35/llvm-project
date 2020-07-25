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
// CHECK:       0: 91000000 add     x0, x0, #0
// CHECK-NEXT:  4: 91000441 add     x1, x2, #1
// CHECK-NEXT:  8: 8b020020 add     x0, x1, x2
// CHECK-NEXT:  c: b1000000 adds    x0, x0, #0
// CHECK-NEXT: 10: ab020020 adds    x0, x1, x2

// CHECK-ASM:      add  x0, x0, #0 // encoding: [0x00,0x00,0x00,0x91]
// CHECK-ASM-NEXT: add  x1, x2, #1 // encoding: [0x41,0x04,0x00,0x91]

// CHECK-ASM:      add  x0, x1, x2 // encoding: [0x20,0x00,0x02,0x8b]

// CHECK-ASM:      adds x0, x0, #0 // encoding: [0x00,0x00,0x00,0xb1]
// CHECK-ASM-NEXT: adds x0, x1, x2 // encoding: [0x20,0x00,0x02,0xab]
