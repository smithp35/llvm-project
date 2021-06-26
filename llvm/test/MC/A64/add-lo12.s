// RUN: llvm-mc -triple a64-elf -filetype=obj %s -o - | llvm-objdump -d -r - | FileCheck %s
// RUN: llvm-mc -triple a64-elf -show-encoding < %s | FileCheck -check-prefix=CHECK-ASM %s

        .global sym
        .local sym2
        .text
        add x0, x1, :lo12: sym
        add x2, x3, :lo12: sym2
sym2:

// CHECK: 0000000000000000 <.text>:
// CHECK-NEXT: 0: 91000020   add     x0, x1, #0
// CHECK-NEXT: 0000000000000000:  R_AARCH64_ADD_ABS_LO12_NC    sym
// CHECK-NEXT: 4: 91000062   add     x2, x3, #0
// CHECK-NEXT: 0000000000000004:  R_AARCH64_ADD_ABS_LO12_NC    .text+0x8

// CHECK-ASM:      add x0, x1, :lo12:sym  // encoding: [0x20,0bAAAAAA00,0b00AAAAAA,0x91]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: :lo12:sym, kind: fixup_a64_add_imm12
// CHECK-ASM-NEXT: add x2, x3, :lo12:sym2 // encoding: [0x62,0bAAAAAA00,0b00AAAAAA,0x91]
// CHECK-ASM-NEXT: //   fixup A - offset: 0, value: :lo12:sym2, kind: fixup_a64_add_imm12
