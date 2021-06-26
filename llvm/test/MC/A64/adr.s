// RUN: llvm-mc -triple a64-elf -filetype=obj %s -o - | llvm-objdump -d -r - | FileCheck %s
// RUN: llvm-mc -triple a64-elf -show-encoding < %s | FileCheck -check-prefix=CHECK-ASM %s

        .global external
        .local local
        .text
        adr x0, 0x1000
        adr x1, external
        adr x3, local
local:  nop
// CHECK: 0000000000000000 <.text>:
// CHECK-NEXT: 0: 10008000   adr     x0, #4096
// CHECK-NEXT: 4: 10000001   adr     x1, #0
// CHECK-NEXT: 0000000000000004:  R_AARCH64_ADR_PREL_LO21      external
// CHECK-NEXT: 8: 10000023   adr     x3, #4
// CHECK-EMPTY:
// CHECK-NEXT: 000000000000000c <local>:
// CHECK-NEXT: c: d503201f   nop

// CHECK-ASM: adr     x0, #4096 // encoding: [0x00,0x80,0x00,0x10]
// CHECK-ASM-NEXT: adr     x1, external // encoding: [0x01'A',A,A,0x10'A']
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: external, kind: fixup_64_pcrel_adr_imm21
// CHECK-ASM-NEXT: adr     x3, local    // encoding: [0x03'A',A,A,0x10'A']
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: local, kind: fixup_64_pcrel_adr_imm21
