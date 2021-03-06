// RUN: llvm-mc -triple a64-elf -filetype=obj %s -o - | llvm-objdump -d -r - | FileCheck %s
// RUN: llvm-mc -triple a64-elf -show-encoding < %s | FileCheck %s --check-prefix=CHECK-ASM
        .text
        madd x0, x1, x2, x14
        msub x11, x12, x13, x15
// CHECK:  0000000000000000 <.text>:
// CHECK-NEXT: 0: 9b023820     madd    x0, x1, x2, x14
// CHECK-NEXT: 4: 9b0dbd8b     msub    x11, x12, x13, x15

// CHECK-ASM: madd x0, x1, x2, x14                    // encoding: [0x20,0x38,0x02,0x9b]
// CHECK-ASM: msub x11, x12, x13, x15                 // encoding: [0x8b,0xbd,0x0d,0x9b]

