// RUN: %clang_cc1 -triple aarch64-linux-gnu                   -emit-llvm %s  -o - | FileCheck %s --check-prefix=OFF
// RUN: %clang_cc1 -triple aarch64-linux-gnu -fptrauth-elf-got -emit-llvm %s  -o - | FileCheck %s --check-prefix=ELFGOT
// RUN: %clang_cc1 -triple aarch64-linux-gnu -fptrauth-calls   -emit-llvm %s  -o - | FileCheck %s --check-prefix=PERSONALITY
// RUN: %clang_cc1 -triple aarch64-none-elf -fptrauth-calls -fptrauth-noakey -emit-llvm %s  -o - | FileCheck %s --check-prefix=PNOAKEY
// RUN: %clang_cc1 -triple aarch64-none-elf -fptrauth-calls -fptrauth-nobkey -emit-llvm %s  -o - | FileCheck %s --check-prefix=PNOBKEY

// ELFGOT:      !llvm.module.flags = !{
// ELFGOT-SAME: !0
// ELFGOT:      !0 = !{i32 1, !"ptrauth-elf-got", i32 1}

// PERSONALITY:      !llvm.module.flags = !{
// PERSONALITY-SAME: !0
// PERSONALITY:      !0 = !{i32 1, !"ptrauth-sign-personality", i32 1}

// PNOAKEY:      !0 = !{i32 1, !"ptrauth-sign-personality", i32 1}
// PNOAKEY:      !1 = !{i32 1, !"ptrauth-noakey", i32 1}

// PNOBKEY:      !0 = !{i32 1, !"ptrauth-sign-personality", i32 1}
// PNOBKEY:      !1 = !{i32 1, !"ptrauth-nobkey", i32 1}

// OFF-NOT: "ptrauth-
