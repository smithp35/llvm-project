// RUN: llvm-mc -triple a64-elf -filetype=obj %s -o - | llvm-objdump -d -r - | FileCheck %s
// RUN: llvm-mc -triple a64-elf -show-encoding < %s | FileCheck --check-prefix CHECK-ASM %s

        .text
        bl 0x1000

// CHECK: 0: 94000400 bl      0x1000 <.text+0x1000>
// CHECK-ASM: bl #4096 // encoding: [0x00,0x04,0x00,0x94]
