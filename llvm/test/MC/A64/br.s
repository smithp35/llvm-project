// RUN: llvm-mc -triple a64-elf -filetype=obj %s -o - | llvm-objdump -d -r - | FileCheck %s
// RUN: llvm-mc -triple a64-elf -show-encoding < %s | FileCheck --check-prefix CHECK-ASM %s

        .text
        br x1

// CHECK: 0000000000000000 <.text>:
// CHECK-NEXT: 0: d61f0020    br      x1

// CHECK-ASM: br x1 // encoding: [0x20,0x00,0x1f,0xd6]
