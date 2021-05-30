// RUN: llvm-mc -triple a64-elf -filetype=obj %s -o - | llvm-objdump -d -r - | FileCheck %s
// RUN: llvm-mc -triple a64-elf -show-encoding < %s | FileCheck --check-prefix CHECK-ASM %s

        .text
        ldr x0, [x1]
        ldr x1, [x2, #0x8]

// CHECK: 0000000000000000 <.text>:
// CHECK-NEXT: 0: f9400020    ldr     x0, [x1]
// CHECK-NEXT: 4: f9400441    ldr     x1, [x2, #8]

// CHECK-ASM:      ldr     x0, [x1]     // encoding: [0x20,0x00,0x40,0xf9]
// CHECK-ASM-NEXT: ldr     x1, [x2, #8] // encoding: [0x41,0x04,0x40,0xf9]
