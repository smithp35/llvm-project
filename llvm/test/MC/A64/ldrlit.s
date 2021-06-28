// RUN: llvm-mc -triple a64-elf -filetype=obj %s -o - | llvm-objdump -d -r - | FileCheck %s
// RUN: llvm-mc -triple a64-elf -show-encoding < %s | FileCheck --check-prefix CHECK-ASM %s

        .text
        .global external
        ldr x0, 0x4
        ldr x1, external
        ldr x2, foo
foo:    .space 8

// CHECK: 0000000000000000 <.text>:
// CHECK-NEXT:        0: 58000020   ldr     x0, 0x4 <.text+0x4>
// CHECK-NEXT:        4: 58000001   ldr     x1, 0x4 <.text+0x4>
// CHECK-NEXT:                 0000000000000004:  R_AARCH64_LD_PREL_LO19       external
// CHECK-NEXT:        8: 58000022   ldr     x2, 0xc <foo>

// CHECK-ASM:      ldr     x0, #4                          // encoding: [0x20,0x00,0x00,0x58]
// CHECK-ASM-NEXT: ldr     x1, external                    // encoding: [0bAAA00001,A,A,0x58]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: external, kind: fixup_a64_ldr_pcrel_imm19
// CHECK-ASM-NEXT: ldr     x2, foo                         // encoding: [0bAAA00010,A,A,0x58]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: foo, kind: fixup_a64_ldr_pcrel_imm19
