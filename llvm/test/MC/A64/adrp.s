// RUN: llvm-mc -triple a64-elf -filetype=obj %s -o - | llvm-objdump -d -r - | FileCheck %s
// RUN: llvm-mc -triple a64-elf -show-encoding < %s | FileCheck -check-prefix=CHECK-ASM %s

        .global external
        .local local
        .text
        adrp x1, external
        adrp x3, local
local:  nop

// CHECK: 0000000000000000 <.text>:
// CHECK-NEXT: 0: 90000001    adrp    x1, 0x0 <.text>
// CHECK-NEXT: 0000000000000000:  R_AARCH64_ADR_PREL_PG_HI21   external
// CHECK-NEXT: 4: 90000003    adrp    x3, 0x0 <.text+0x4>
// CHECK-NEXT: 0000000000000004:  R_AARCH64_ADR_PREL_PG_HI21   .text+0x8

// CHECK-ASM:      adrp    x1, external // encoding: [0x01'A',A,A,0x90'A']
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: external, kind: fixup_64_pcrel_adrp_imm21
// CHECK-ASM-NEXT: adrp    x3, local // encoding: [0x03'A',A,A,0x90'A']
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: local, kind: fixup_64_pcrel_adrp_imm21
