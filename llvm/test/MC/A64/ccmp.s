// RUN: llvm-mc -triple a64-elf -filetype=obj %s -o - | llvm-objdump -d -r - | FileCheck %s
// RUN: llvm-mc -triple a64-elf -show-encoding < %s | FileCheck -check-prefix=CHECK-ASM %s
  .text
  ccmn x1, #2, #3, eq
  ccmp x1, #2, #3, eq

  ccmn x1, x2, #3, eq
  ccmp x1, x2, #3, eq

// CHECK:      0: ba420823   ccmn    x1, #2, #3, eq
// CHECK-NEXT: 4: fa420823   ccmp    x1, #2, #3, eq
// CHECK-NEXT: 8: ba420023   ccmn    x1, x2, #3, eq
// CHECK-NEXT: c: fa420023   ccmp    x1, x2, #3, eq

// CHECK-ASM:      ccmn x1, #2, #3, eq // encoding: [0x23,0x08,0x42,0xba]
// CHECK-ASM-NEXT: ccmp x1, #2, #3, eq // encoding: [0x23,0x08,0x42,0xfa]

// CHECK-ASM:      ccmn x1, x2, #3, eq // encoding: [0x23,0x00,0x42,0xba]
// CHECK-ASM-NEXT: ccmp x1, x2, #3, eq // encoding: [0x23,0x00,0x42,0xfa]
