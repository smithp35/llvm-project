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

/// logical shifts
        and x0, x0, x1, lsl 1
        bic x11, x12, x13, lsr 2
        orr x1, x2, x4, asr 3
        orn x5, x6, x7, ror 4
        eor x3, x4, x5, lsl 63
        eon x7, x8, x9, asr 62

        ands x0, x0, x1, lsl 1
        bics x2, x4, x5, lsr 2
        ands x0, x0, x1, asr 3
        bics x2, x4, x5, ror 4

// CHECK:       0: 8a010000    and     x0, x0, x1
// CHECK-NEXT:  4: 8a2d018b    bic     x11, x12, x13
// CHECK-NEXT:  8: aa040041    orr     x1, x2, x4
// CHECK-NEXT:  c: aa2700c5    orn     x5, x6, x7
// CHECK-NEXT: 10: ca050083    eor     x3, x4, x5
// CHECK-NEXT: 14: ca290107    eon     x7, x8, x9
// CHECK-NEXT: 18: ea010000    ands    x0, x0, x1
// CHECK-NEXT: 1c: ea250082    bics    x2, x4, x5
// CHECK-NEXT: 20: 8a010400    and     x0, x0, x1, lsl #1
// CHECK-NEXT: 24: 8a6d098b    bic     x11, x12, x13, lsr #2
// CHECK-NEXT: 28: aa840c41    orr     x1, x2, x4, asr #3
// CHECK-NEXT: 2c: aae710c5    orn     x5, x6, x7, ror #4
// CHECK-NEXT: 30: ca05fc83    eor     x3, x4, x5, lsl #63
// CHECK-NEXT: 34: caa9f907    eon     x7, x8, x9, asr #62
// CHECK-NEXT: 38: ea010400   ands    x0, x0, x1, lsl #1
// CHECK-NEXT: 3c: ea650882   bics    x2, x4, x5, lsr #2
// CHECK-NEXT: 40: ea810c00   ands    x0, x0, x1, asr #3
// CHECK-NEXT: 44: eae51082   bics    x2, x4, x5, ror #4

// CHECK-ASM: and x0, x0, x1         // encoding: [0x00,0x00,0x01,0x8a]
// CHECK-ASM-NEXT: bic x11, x12, x13 // encoding: [0x8b,0x01,0x2d,0x8a]
// CHECK-ASM-NEXT: orr x1, x2, x4    // encoding: [0x41,0x00,0x04,0xaa]
// CHECK-ASM-NEXT: orn x5, x6, x7    // encoding: [0xc5,0x00,0x27,0xaa]
// CHECK-ASM-NEXT: eor x3, x4, x5    // encoding: [0x83,0x00,0x05,0xca]
// CHECK-ASM-NEXT: eon x7, x8, x9    // encoding: [0x07,0x01,0x29,0xca]

// CHECK-ASM: ands x0, x0, x1        // encoding: [0x00,0x00,0x01,0xea]
// CHECK-ASM-NEXT: bics x2, x4, x5   // encoding: [0x82,0x00,0x25,0xea]

// CHECK-ASM: and x0, x0, x1, lsl #1        // encoding: [0x00,0x04,0x01,0x8a]
// CHECK-ASM-NEXT: bic x11, x12, x13, lsr #2 // encoding: [0x8b,0x09,0x6d,0x8a]
// CHECK-ASM-NEXT: orr x1, x2, x4, asr #3   // encoding: [0x41,0x0c,0x84,0xaa]
// CHECK-ASM-NEXT: orn x5, x6, x7, ror #4   // encoding: [0xc5,0x10,0xe7,0xaa]
// CHECK-ASM-NEXT: eor x3, x4, x5, lsl #63  // encoding: [0x83,0xfc,0x05,0xca]
// CHECK-ASM-NEXT: eon x7, x8, x9, asr #62  // encoding: [0x07,0xf9,0xa9,0xca]
// CHECK-ASM:ands x0, x0, x1, lsl #1        // encoding: [0x00,0x04,0x01,0xea]
// CHECK-ASM-NEXT: bics x2, x4, x5, lsr #2  // encoding: [0x82,0x08,0x65,0xea]
// CHECK-ASM-NEXT: ands x0, x0, x1, asr #3  // encoding: [0x00,0x0c,0x81,0xea]
// CHECK-ASM-NEXT: bics x2, x4, x5, ror #4  // encoding: [0x82,0x10,0xe5,0xea]
