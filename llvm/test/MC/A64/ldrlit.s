// RUN: llvm-mc -triple a64-elf -filetype=obj %s -o - | llvm-objdump -d -r - | FileCheck %s
// RUN: llvm-mc -triple a64-elf -show-encoding < %s | FileCheck --check-prefix CHECK-ASM %s

        .text
        ldr x0, 0x4
foo:    .space 8

// CHECK: 0: 58000020   ldr     x0, 0x4 <foo>

// CHECK-ASM: ldr x0, #4 // encoding: [0x20,0x00,0x00,0x58]
