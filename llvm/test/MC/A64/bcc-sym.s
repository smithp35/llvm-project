// RUN: llvm-mc -triple a64-elf -filetype=obj %s -o - | llvm-objdump -d -r - | FileCheck %s
// RUN: llvm-mc -triple a64-elf -show-encoding < %s | FileCheck --check-prefix CHECK-ASM %s

        .text
        .global external
        b.eq external
        .local local
local:  b.ne local

// CHECK: 0000000000000000 <.text>:
// CHECK-NEXT: 0: 54000000   b.eq    0x0 <.text>
// CHECK-NEXT: 0000000000000000:  R_AARCH64_CONDBR19   external
// CHECK-EMPTY: 
// CHECK-NEXT: 0000000000000004 <local>:
// CHECK-NEXT: 4: 54000001   b.ne    0x4 <local>

// CHECK-ASM: b.eq    external // encoding: [0bAAA00000,A,A,0x54]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: external, kind: fixup_a64_pcrel_branch19

// CHECK-ASM: local:
// CHECK-ASM-NEXT: b.ne    local // encoding: [0bAAA00001,A,A,0x54]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: local, kind: fixup_a64_pcrel_branch19
