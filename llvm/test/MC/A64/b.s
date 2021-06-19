// RUN: llvm-mc -triple a64-elf -filetype=obj %s -o - | llvm-objdump -d -r - | FileCheck %s
// RUN: llvm-mc -triple a64-elf -show-encoding < %s | FileCheck --check-prefix CHECK-ASM %s

        .text
        b 0x1000

        .global external
        b external
        .local local
local:  b local

// CHECK: 0000000000000000 <.text>:
// CHECK-NEXT:        0: 14000400  	b	0x1000
// CHECK-NEXT:        4: 14000000  	b	0x4
// CHECK-NEXT: 		0000000000000004:  R_AARCH64_JUMP26	external
// CHECK-EMPTY:
// CHECK-NEXT: 0000000000000008 <local>:
// CHECK-NEXT:        8: 14000000  	b	0x8 <local>

// CHECK-ASM: b #4096 // encoding: [0x00,0x04,0x00,0x14]
// CHECK-ASM: b external // encoding: [A,A,A,0b000101AA]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: external, kind: fixup_a64_pcrel_branch26
// CHECK-ASM: b local                           // encoding: [A,A,A,0b000101AA]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: local, kind: fixup_a64_pcrel_branch26

