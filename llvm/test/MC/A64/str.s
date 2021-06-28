// RUN: llvm-mc -triple a64-elf -filetype=obj %s -o - | llvm-objdump -d -r - > /tmp/t.txt
// RUN: llvm-mc -triple a64-elf -show-encoding < %s > /tmp/t2.txt
// RUN llvm-mc -triple a64-elf -filetype=obj %s -o - | llvm-objdump -d -r - | FileCheck %s
// RUN llvm-mc -triple a64-elf -show-encoding < %s | FileCheck --check-prefix CHECK-ASM %s

        .text
        str x0, [x1]
        str x1, [x2, #0x8]

// CHECK: 0000000000000000 <.text>:
// CHECK-NEXT: 0: f9000020    str     x0, [x1]
// CHECK-NEXT: 4: f9000441    str     x1, [x2, #8]

// CHECK-ASM:      str x0, [x1]     // encoding: [0x20,0x00,0x00,0xf9]
// CHECK-ASM-NEXT: str x1, [x2, #8] // encoding: [0x41,0x04,0x00,0xf9]
