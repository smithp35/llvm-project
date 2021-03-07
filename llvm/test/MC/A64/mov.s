// RUN: llvm-mc -triple a64-elf -filetype=obj %s -o - | llvm-objdump -d -r - | FileCheck %s
// RUN: llvm-mc -triple a64-elf -show-encoding < %s | FileCheck --check-prefix CHECK-ASM %s
        .text

/// Register form
        mov x0, x1
        mov x15, x16

// CHECK: 0000000000000000 <.text>:
// CHECK-NEXT:        0: aa0103e0   mov     x0, x1
// CHECK-NEXT:        4: aa1003ef   mov     x15, x16

// CHECK-ASM:         mov x0, x1                              // encoding: [0xe0,0x03,0x01,0xaa]
// CHECK-ASM-NEXT:         mov x15, x16                            // encoding: [0xef,0x03,0x10,0xaa]
