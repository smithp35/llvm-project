// RUN: llvm-mc -triple a64-elf -filetype=obj %s -o - | llvm-objdump -d -r - | FileCheck %s
// RUN: llvm-mc -triple a64-elf -show-encoding < %s | FileCheck -check-prefix=CHECK-ASM %s
        .text
/// register form only
        and x0, x0, x1
        bic x11, x12, x13
        orr x1, x2, x4
        orn x5, x6, x7
        eor x3, x4, x5
        eon x7, x8, x9

/// flag setting
        ands x0, x0, x1
        bics x2, x4, x5

// CHECK:       0: 8a010000    and     x0, x0, x1
// CHECK-NEXT:  4: 8a2d018b    bic     x11, x12, x13
// CHECK-NEXT:  8: aa040041    orr     x1, x2, x4
// CHECK-NEXT:  c: aa2700c5    orn     x5, x6, x7
// CHECK-NEXT: 10: ca050083    eor     x3, x4, x5
// CHECK-NEXT: 14: ca290107    eon     x7, x8, x9
// CHECK-NEXT: 18: ea010000    ands    x0, x0, x1
// CHECK-NEXT: 1c: ea250082    bics    x2, x4, x5

// CHECK-ASM: and x0, x0, x1         // encoding: [0x00,0x00,0x01,0x8a]
// CHECK-ASM-NEXT: bic x11, x12, x13 // encoding: [0x8b,0x01,0x2d,0x8a]
// CHECK-ASM-NEXT: orr x1, x2, x4    // encoding: [0x41,0x00,0x04,0xaa]
// CHECK-ASM-NEXT: orn x5, x6, x7    // encoding: [0xc5,0x00,0x27,0xaa]
// CHECK-ASM-NEXT: eor x3, x4, x5    // encoding: [0x83,0x00,0x05,0xca]
// CHECK-ASM-NEXT: eon x7, x8, x9    // encoding: [0x07,0x01,0x29,0xca]

// CHECK-ASM: ands x0, x0, x1        // encoding: [0x00,0x00,0x01,0xea]
// CHECK-ASM-NEXT: bics x2, x4, x5   // encoding: [0x82,0x00,0x25,0xea]
