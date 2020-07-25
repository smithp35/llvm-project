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

// CHECK:       0: d1000000    sub     x0, x0, #0
// CHECK-NEXT:  4: d1000441    sub     x1, x2, #1
// CHECK-NEXT:  8: cb020020    sub     x0, x1, x2
// CHECK-NEXT:  c: f1000000    subs    x0, x0, #0
// CHECK-NEXT: 10: eb020020    subs    x0, x1, x2

// CHECK-ASM:  sub x0, x0, #0 // encoding: [0x00,0x00,0x00,0xd1]
// CHECK-ASM-NEXT:  sub x1, x2, #1 // encoding: [0x41,0x04,0x00,0xd1]

// CHECK-ASM:  sub x0, x1, x2 // encoding: [0x20,0x00,0x02,0xcb]

// CHECK-ASM:  subs x0, x0, #0 // encoding: [0x00,0x00,0x00,0xf1]
// CHECK-ASM-NEXT:  subs x0, x1, x2 // encoding: [0x20,0x00,0x02,0xeb]
