// RUN: llvm-mc -triple a64-elf -filetype=obj %s -o - | llvm-objdump -d -r - | FileCheck %s
// RUN: llvm-mc -triple a64-elf -show-encoding < %s | FileCheck -check-prefix=CHECK-ASM %s
        .text
        nop

/// Simple first step to check the encoding and disassembly of nop
// CHECK: 0: d503201f         nop

// CHECK-ASM:      .text
// CHECK-ASM-NEXT: nop // encoding: [0x1f,0x20,0x03,0xd5]
