// RUN: llvm-mc -triple a64-elf -filetype=obj %s -o - | llvm-objdump -d -r - | FileCheck %s
// RUN: llvm-mc -triple a64-elf -show-encoding < %s | FileCheck --check-prefix CHECK-ASM %s

        .text

        movz x0, #0
        movn x1, #2
        movk x2, #4

// CHECK: 0000000000000000 <.text>:
// CHECK-NEXT: 0: d2800000    mov     x0, #0
// CHECK-NEXT: 4: 92800041    mov     x1, #-3
// CHECK-NEXT: 8: f2800082    movk    x2, #4

// CHECK-ASM:      movz    x0, #0 // encoding: [0x00,0x00,0x80,0xd2]
// CHECK-ASM-NEXT: movn    x1, #2 // encoding: [0x41,0x00,0x80,0x92]
// CHECK-ASM-NEXT: movk x2, #4    // encoding: [0x82,0x00,0x80,0xf2]
