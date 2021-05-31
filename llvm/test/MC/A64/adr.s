// RUN: llvm-mc -triple a64-elf -filetype=obj %s -o - | llvm-objdump -d -r - | FileCheck %s
// RUN: llvm-mc -triple a64-elf -show-encoding < %s | FileCheck -check-prefix=CHECK-ASM %s

        .text
        adr x0, 0x1000

// CHECK: 0000000000000000 <.text>:
// CHECK-NEXT: 0: 10008000   adr     x0, #4096

// CHECK-ASM: adr     x0, #4096 // encoding: [0x00,0x80,0x00,0x10]
