// RUN: llvm-mc -triple a64-elf -filetype=obj %s -o - | llvm-objdump -d -r - | FileCheck %s
// RUN: llvm-mc -triple a64-elf -show-encoding < %s | FileCheck --check-prefix CHECK-ASM %s

        .text
        ret x2
        ret

// CHECK: 0000000000000000 <.text>:
// CHECK-NEXT: 0: d65f0040    ret     x2
// CHECK-NEXT: 4: d65f03c0    ret

// CHECK-ASM:      ret     x2 // encoding: [0x40,0x00,0x5f,0xd6]
// CHECK-ASM-NEXT: ret        // encoding: [0xc0,0x03,0x5f,0xd6]
