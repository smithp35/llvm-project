// RUN: llvm-mc -triple a64-elf -filetype=obj %s -o - | llvm-objdump -d -r - | FileCheck %s
// RUN: llvm-mc -triple a64-elf -show-encoding < %s | FileCheck --check-prefix CHECK-ASM %s

        .text
        beq 0x1000
        b.eq 0x1000
        bne 0x1000
        b.ne 0x1000
        bhs 0x1000
        b.hs 0x1000
        bcs 0x1000
        b.cs 0x1000
        blo 0x1000
        b.lo 0x1000
        bcc 0x1000
        b.cc 0x1000
        bmi 0x1000
        b.mi 0x1000
        bpl 0x1000
        b.pl 0x1000
        bvs 0x1000
        b.vs 0x1000
        bvc 0x1000
        b.vc 0x1000
        bhi 0x1000
        b.hi 0x1000
        bls 0x1000
        b.ls 0x1000
        bge 0x1000
        b.ge 0x1000
        blt 0x1000
        b.lt 0x1000
        bgt 0x1000
        b.gt 0x1000
        ble 0x1000
        b.le 0x1000
        bal 0x1000
        b.al 0x1000
        bnv 0x1000
        b.nv 0x1000


// CHECK:        0: 54008000   b.eq    0x1000 <.text+0x1000>
// CHECK-NEXT:   4: 54008000   b.eq    0x1004 <.text+0x1004>
// CHECK-NEXT:   8: 54008001   b.ne    0x1008 <.text+0x1008>
// CHECK-NEXT:   c: 54008001   b.ne    0x100c <.text+0x100c>
// CHECK-NEXT:  10: 54008002   b.hs    0x1010 <.text+0x1010>
// CHECK-NEXT:  14: 54008002   b.hs    0x1014 <.text+0x1014>
// CHECK-NEXT:  18: 54008002   b.hs    0x1018 <.text+0x1018>
// CHECK-NEXT:  1c: 54008002   b.hs    0x101c <.text+0x101c>
// CHECK-NEXT:  20: 54008003   b.lo    0x1020 <.text+0x1020>
// CHECK-NEXT:  24: 54008003   b.lo    0x1024 <.text+0x1024>
// CHECK-NEXT:  28: 54008003   b.lo    0x1028 <.text+0x1028>
// CHECK-NEXT:  2c: 54008003   b.lo    0x102c <.text+0x102c>
// CHECK-NEXT:  30: 54008004   b.mi    0x1030 <.text+0x1030>
// CHECK-NEXT:  34: 54008004   b.mi    0x1034 <.text+0x1034>
// CHECK-NEXT:  38: 54008005   b.pl    0x1038 <.text+0x1038>
// CHECK-NEXT:  3c: 54008005   b.pl    0x103c <.text+0x103c>
// CHECK-NEXT:  40: 54008006   b.vs    0x1040 <.text+0x1040>
// CHECK-NEXT:  44: 54008006   b.vs    0x1044 <.text+0x1044>
// CHECK-NEXT:  48: 54008007   b.vc    0x1048 <.text+0x1048>
// CHECK-NEXT:  4c: 54008007   b.vc    0x104c <.text+0x104c>
// CHECK-NEXT:  50: 54008008   b.hi    0x1050 <.text+0x1050>
// CHECK-NEXT:  54: 54008008   b.hi    0x1054 <.text+0x1054>
// CHECK-NEXT:  58: 54008009   b.ls    0x1058 <.text+0x1058>
// CHECK-NEXT:  5c: 54008009   b.ls    0x105c <.text+0x105c>
// CHECK-NEXT:  60: 5400800a   b.ge    0x1060 <.text+0x1060>
// CHECK-NEXT:  64: 5400800a   b.ge    0x1064 <.text+0x1064>
// CHECK-NEXT:  68: 5400800b   b.lt    0x1068 <.text+0x1068>
// CHECK-NEXT:  6c: 5400800b   b.lt    0x106c <.text+0x106c>
// CHECK-NEXT:  70: 5400800c   b.gt    0x1070 <.text+0x1070>
// CHECK-NEXT:  74: 5400800c   b.gt    0x1074 <.text+0x1074>
// CHECK-NEXT:  78: 5400800d   b.le    0x1078 <.text+0x1078>
// CHECK-NEXT:  7c: 5400800d   b.le    0x107c <.text+0x107c>
// CHECK-NEXT:  80: 5400800e   b.al    0x1080 <.text+0x1080>
// CHECK-NEXT:  84: 5400800e   b.al    0x1084 <.text+0x1084>
// CHECK-NEXT:  88: 5400800f   b.nv    0x1088 <.text+0x1088>
// CHECK-NEXT:  8c: 5400800f   b.nv    0x108c <.text+0x108c>

// CHECK-ASM:      b.eq    #4096 // encoding: [0x00,0x80,0x00,0x54]
// CHECK-ASM-NEXT: b.eq    #4096 // encoding: [0x00,0x80,0x00,0x54]
// CHECK-ASM-NEXT: b.ne    #4096 // encoding: [0x01,0x80,0x00,0x54]
// CHECK-ASM-NEXT: b.ne    #4096 // encoding: [0x01,0x80,0x00,0x54]
// CHECK-ASM-NEXT: b.hs    #4096 // encoding: [0x02,0x80,0x00,0x54]
// CHECK-ASM-NEXT: b.hs    #4096 // encoding: [0x02,0x80,0x00,0x54]
// CHECK-ASM-NEXT: b.hs    #4096 // encoding: [0x02,0x80,0x00,0x54]
// CHECK-ASM-NEXT: b.hs    #4096 // encoding: [0x02,0x80,0x00,0x54]
// CHECK-ASM-NEXT: b.lo    #4096 // encoding: [0x03,0x80,0x00,0x54]
// CHECK-ASM-NEXT: b.lo    #4096 // encoding: [0x03,0x80,0x00,0x54]
// CHECK-ASM-NEXT: b.lo    #4096 // encoding: [0x03,0x80,0x00,0x54]
// CHECK-ASM-NEXT: b.lo    #4096 // encoding: [0x03,0x80,0x00,0x54]
// CHECK-ASM-NEXT: b.mi    #4096 // encoding: [0x04,0x80,0x00,0x54]
// CHECK-ASM-NEXT: b.mi    #4096 // encoding: [0x04,0x80,0x00,0x54]
// CHECK-ASM-NEXT: b.pl    #4096 // encoding: [0x05,0x80,0x00,0x54]
// CHECK-ASM-NEXT: b.pl    #4096 // encoding: [0x05,0x80,0x00,0x54]
// CHECK-ASM-NEXT: b.vs    #4096 // encoding: [0x06,0x80,0x00,0x54]
// CHECK-ASM-NEXT: b.vs    #4096 // encoding: [0x06,0x80,0x00,0x54]
// CHECK-ASM-NEXT: b.vc    #4096 // encoding: [0x07,0x80,0x00,0x54]
// CHECK-ASM-NEXT: b.vc    #4096 // encoding: [0x07,0x80,0x00,0x54]
// CHECK-ASM-NEXT: b.hi    #4096 // encoding: [0x08,0x80,0x00,0x54]
// CHECK-ASM-NEXT: b.hi    #4096 // encoding: [0x08,0x80,0x00,0x54]
// CHECK-ASM-NEXT: b.ls    #4096 // encoding: [0x09,0x80,0x00,0x54]
// CHECK-ASM-NEXT: b.ls    #4096 // encoding: [0x09,0x80,0x00,0x54]
// CHECK-ASM-NEXT: b.ge    #4096 // encoding: [0x0a,0x80,0x00,0x54]
// CHECK-ASM-NEXT: b.ge    #4096 // encoding: [0x0a,0x80,0x00,0x54]
// CHECK-ASM-NEXT: b.lt    #4096 // encoding: [0x0b,0x80,0x00,0x54]
// CHECK-ASM-NEXT: b.lt    #4096 // encoding: [0x0b,0x80,0x00,0x54]
// CHECK-ASM-NEXT: b.gt    #4096 // encoding: [0x0c,0x80,0x00,0x54]
// CHECK-ASM-NEXT: b.gt    #4096 // encoding: [0x0c,0x80,0x00,0x54]
// CHECK-ASM-NEXT: b.le    #4096 // encoding: [0x0d,0x80,0x00,0x54]
// CHECK-ASM-NEXT: b.le    #4096 // encoding: [0x0d,0x80,0x00,0x54]
// CHECK-ASM-NEXT: b.al    #4096 // encoding: [0x0e,0x80,0x00,0x54]
// CHECK-ASM-NEXT: b.al    #4096 // encoding: [0x0e,0x80,0x00,0x54]
// CHECK-ASM-NEXT: b.nv    #4096 // encoding: [0x0f,0x80,0x00,0x54]
// CHECK-ASM-NEXT: b.nv    #4096 // encoding: [0x0f,0x80,0x00,0x54]
