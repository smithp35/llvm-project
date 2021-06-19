// RUN: llvm-mc -triple a64-elf -filetype=obj %s -o - | llvm-objdump -d -r - | FileCheck %s
// RUN: llvm-mc -triple a64-elf -show-encoding < %s | FileCheck --check-prefix CHECK-ASM %s

        .text
        bl 0x1000

        .global external
        bl external
        .local local
local:  bl local

// CHECK: 0000000000000000 <.text>:
// CHECK-NEXT: 0: 94000400   bl      0x1000 <local+0xff8>
// CHECK-NEXT: 4: 94000000   bl      0x4 <.text+0x4>
// CHECK-NEXT: 0000000000000004:  R_AARCH64_CALL26     external
// CHECK-EMPTY:
// CHECK-NEXT: 0000000000000008 <local>:
// CHECK-NEXT: 8: 94000000   bl      0x8 <local>

// CHECK-ASM: bl #4096 // encoding: [0x00,0x04,0x00,0x94]
// CHECK-ASM: bl external // encoding: [A,A,A,0b100101AA]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: external, kind: fixup_a64_pcrel_call26
// CHECK-ASM: bl local    // encoding: [A,A,A,0b100101AA]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: local, kind: fixup_a64_pcrel_call26
