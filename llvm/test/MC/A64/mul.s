// RUN: llvm-mc -triple a64-elf -filetype=obj %s -o - | llvm-objdump -d -r - | FileCheck %s
// RUN: llvm-mc -triple a64-elf -show-encoding < %s | FileCheck %s --check-prefix CHECK-ASM
        .text
        mul x0, x1, x2
// CHECK: 0000000000000000 <.text>:
// CHECK-NEXT:        0: 9b027c20   mul     x0, x1, x2

// CHECK-ASM:         mul x0, x1, x2                          // encoding: [0x20,0x7c,0x02,0x9b]



