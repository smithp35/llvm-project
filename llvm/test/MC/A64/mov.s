// RUN: llvm-mc -triple a64-elf -filetype=obj %s -o - | llvm-objdump -d -r - | FileCheck %s
// RUN: llvm-mc -triple a64-elf -show-encoding < %s | FileCheck --check-prefix CHECK-ASM %s
        .text

/// Register form
        mov x0, x1
        mov x15, x16
        mvn x2, x3
        mvn x4,x5

        /// Immediate form
        mov x0, #0xff0000000

// CHECK: 0000000000000000 <.text>:
// CHECK-NEXT:  0: aa0103e0   mov     x0, x1
// CHECK-NEXT:  4: aa1003ef   mov     x15, x16
// CHECK-NEXT:  8: aa2303e2    mvn     x2, x3
// CHECK-NEXT:  c: aa2503e4    mvn     x4, x5
// CHECK-NEXT: 10: b2641fe0    mov     x0, #68451041280

// CHECK-ASM:      mov x0, x1   // encoding: [0xe0,0x03,0x01,0xaa]
// CHECK-ASM-NEXT: mov x15, x16 // encoding: [0xef,0x03,0x10,0xaa]
// CHECK-ASM-NEXT: mvn  x2, x3  // encoding: [0xe2,0x03,0x23,0xaa]
// CHECK-ASM-NEXT: mvn  x4, x5  // encoding: [0xe4,0x03,0x25,0xaa]
// CHECK-ASM:     orr x0, xzr, #0xff0000000 // encoding: [0xe0,0x1f,0x64,0xb2]
