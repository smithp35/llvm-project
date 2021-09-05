// RUN: llvm-mc -triple a64-elf -filetype=obj %s -o - | llvm-objdump -d -r - | FileCheck %s
// RUN: llvm-mc -triple a64-elf -show-encoding < %s | FileCheck -check-prefix=CHECK-ASM %s

        .text
        csel x0, x1, x2, eq
        csel x3, x4, x5, ne
        csel x2, x3, x4, hs
        csel x0, x1, x2, cs
        csel x3, x4, x5, lo
        csel x2, x3, x4, cc
        csel x0, x1, x2, mi
        csel x3, x4, x5, pl
        csel x2, x3, x4, vs
        csel x0, x1, x2, vc
        csel x3, x4, x5, hi
        csel x2, x3, x4, ls
        csel x0, x1, x2, ge
        csel x3, x4, x5, lt
        csel x2, x3, x4, gt
        csel x2, x3, x4, le

// CHECK:       0: 9a820020   csel    x0, x1, x2, eq
// CHECK-NEXT:  4: 9a851083   csel    x3, x4, x5, ne
// CHECK-NEXT:  8: 9a842062   csel    x2, x3, x4, hs
// CHECK-NEXT:  c: 9a822020   csel    x0, x1, x2, hs
// CHECK-NEXT: 10: 9a853083   csel    x3, x4, x5, lo
// CHECK-NEXT: 14: 9a843062   csel    x2, x3, x4, lo
// CHECK-NEXT: 18: 9a824020   csel    x0, x1, x2, mi
// CHECK-NEXT: 1c: 9a855083   csel    x3, x4, x5, pl
// CHECK-NEXT: 20: 9a846062   csel    x2, x3, x4, vs
// CHECK-NEXT: 24: 9a827020   csel    x0, x1, x2, vc
// CHECK-NEXT: 28: 9a858083   csel    x3, x4, x5, hi
// CHECK-NEXT: 2c: 9a849062   csel    x2, x3, x4, ls
// CHECK-NEXT: 30: 9a82a020   csel    x0, x1, x2, ge
// CHECK-NEXT: 34: 9a85b083   csel    x3, x4, x5, lt
// CHECK-NEXT: 38: 9a84c062   csel    x2, x3, x4, gt
// CHECK-NEXT: 3c: 9a84d062   csel    x2, x3, x4, le

// CHECK-ASM:      csel    x0, x1, x2, eq // encoding: [0x20,0x00,0x82,0x9a]
// CHECK-ASM-NEXT: csel    x3, x4, x5, ne // encoding: [0x83,0x10,0x85,0x9a]
// CHECK-ASM-NEXT: csel    x2, x3, x4, hs // encoding: [0x62,0x20,0x84,0x9a]
// CHECK-ASM-NEXT: csel    x0, x1, x2, hs // encoding: [0x20,0x20,0x82,0x9a]
// CHECK-ASM-NEXT: csel    x3, x4, x5, lo // encoding: [0x83,0x30,0x85,0x9a]
// CHECK-ASM-NEXT: csel    x2, x3, x4, lo // encoding: [0x62,0x30,0x84,0x9a]
// CHECK-ASM-NEXT: csel    x0, x1, x2, mi // encoding: [0x20,0x40,0x82,0x9a]
// CHECK-ASM-NEXT: csel    x3, x4, x5, pl // encoding: [0x83,0x50,0x85,0x9a]
// CHECK-ASM-NEXT: csel    x2, x3, x4, vs // encoding: [0x62,0x60,0x84,0x9a]
// CHECK-ASM-NEXT: csel    x0, x1, x2, vc // encoding: [0x20,0x70,0x82,0x9a]
// CHECK-ASM-NEXT: csel    x3, x4, x5, hi // encoding: [0x83,0x80,0x85,0x9a]
// CHECK-ASM-NEXT: csel    x2, x3, x4, ls // encoding: [0x62,0x90,0x84,0x9a]
// CHECK-ASM-NEXT: csel    x0, x1, x2, ge // encoding: [0x20,0xa0,0x82,0x9a]
// CHECK-ASM-NEXT: csel    x3, x4, x5, lt // encoding: [0x83,0xb0,0x85,0x9a]
// CHECK-ASM-NEXT: csel    x2, x3, x4, gt // encoding: [0x62,0xc0,0x84,0x9a]
// CHECK-ASM-NEXT: csel    x2, x3, x4, le // encoding: [0x62,0xd0,0x84,0x9a]
