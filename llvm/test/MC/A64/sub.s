// RUN: llvm-mc -triple a64-elf -filetype=obj %s -o - | llvm-objdump -d -r - | FileCheck %s
// RUN: llvm-mc -triple a64-elf -show-encoding < %s | FileCheck -check-prefix=CHECK-ASM %s

        .text
/// Immediate form
        sub x0, x0, #0
        sub x1, x2, #1

/// Register form
        sub x0, x1, x2

/// Flag setting variant
        subs x0, x0, #0
        subs x0, x1, x2

// Shifted register form
        sub x3, x4, x5, lsl 1
        sub x8, x20, x21, lsr 33
        sub x9, x30, x29, asr 63

        subs x3, x4, x5, lsl 1
        subs x8, x20, x21, lsr 33
        subs x9, x30, x29, asr 63

// Shifted immediate form
        sub x0, x0, #0, lsl #0
        sub x1, x2, #1, lsl #12
        subs x0, x0, #0, lsl #0
        subs x1, x2, #1, lsl #12


// CHECK:       0: d1000000 sub     x0, x0, #0
// CHECK-NEXT:  4: d1000441 sub     x1, x2, #1
// CHECK-NEXT:  8: cb020020 sub     x0, x1, x2
// CHECK-NEXT:  c: f1000000 subs    x0, x0, #0
// CHECK-NEXT: 10: eb020020 subs    x0, x1, x2
// CHECK-NEXT: 14: cb050483 sub     x3, x4, x5, lsl #1
// CHECK-NEXT: 18: cb558688 sub     x8, x20, x21, lsr #33
// CHECK-NEXT: 1c: cb9dffc9 sub     x9, x30, x29, asr #63
// CHECK-NEXT: 20: eb050483 subs    x3, x4, x5, lsl #1
// CHECK-NEXT: 24: eb558688 subs    x8, x20, x21, lsr #33
// CHECK-NEXT: 28: eb9dffc9 subs     x9, x30, x29, asr #63
// CHECK-NEXT: 2c: d1000000 sub     x0, x0, #0
// CHECK-NEXT: 30: d1400441 sub     x1, x2, #1, lsl #12
// CHECK-NEXT: 34: f1000000 subs    x0, x0, #0
// CHECK-NEXT: 38: f1400441 subs    x1, x2, #1, lsl #12

// CHECK-ASM:  sub x0, x0, #0 // encoding: [0x00,0x00,0x00,0xd1]
// CHECK-ASM-NEXT: // =0
// CHECK-ASM-NEXT:  sub x1, x2, #1 // encoding: [0x41,0x04,0x00,0xd1]
// CHECK-ASM-NEXT: // =1

// CHECK-ASM:  sub x0, x1, x2 // encoding: [0x20,0x00,0x02,0xcb]

// CHECK-ASM:  subs x0, x0, #0 // encoding: [0x00,0x00,0x00,0xf1]
// CHECK-ASM-NEXT: // =0
// CHECK-ASM-NEXT:  subs x0, x1, x2 // encoding: [0x20,0x00,0x02,0xeb]

// CHECK-ASM: sub x3, x4, x5, lsl #1 // encoding: [0x83,0x04,0x05,0xcb]
// CHECK-ASM-NEXT: sub x8, x20, x21, lsr #33 // encoding: [0x88,0x86,0x55,0xcb]
// CHECK-ASM-NEXT: sub x9, x30, x29, asr #63 // encoding: [0xc9,0xff,0x9d,0xcb]

// CHECK-ASM: subs x3, x4, x5, lsl #1       // encoding: [0x83,0x04,0x05,0xeb]
// CHECK-ASM-NEXT: subs x8, x20, x21, lsr #33 // encoding: [0x88,0x86,0x55,0xeb]
// CHECK-ASM-NEXT: subs x9, x30, x29, asr #63 // encoding: [0xc9,0xff,0x9d,0xeb]

// CHECK-ASM: sub x0, x0, #0          // encoding: [0x00,0x00,0x00,0xd1]
// CHECK-ASM-NEXT: // =0
// CHECK-ASM-NEXT: sub x1, x2, #1, lsl #12 // encoding: [0x41,0x04,0x40,0xd1]
// CHECK-ASM-NEXT: // =4096
// CHECK-ASM-NEXT: subs x0, x0, #0         // encoding: [0x00,0x00,0x00,0xf1]
// CHECK-ASM-NEXT: // =0
// CHECK-ASM-NEXT: subs x1, x2, #1, lsl #12 // encoding: [0x41,0x04,0x40,0xf1]
// CHECK-ASM-NEXT: // =4096
