// REQUIRES: aarch64-registered-target

/// Custom profile defaults to the minimum viable signing schema
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=custom -mpauthabi-platform=0x0100000000000000 -mpauthabi-version=1 %s 2>&1 | FileCheck %s --check-prefix CUSTOM
// RUN: %clang -### -c --target=aarch64-linux-gnu -march=armv8.3-a -mpauthabi-profile=custom -mpauthabi-platform=0x0100000000000000 -mpauthabi-version=1 %s 2>&1 | FileCheck %s --check-prefix CUSTOM
// CUSTOM: "-cc1"{{.*}} "-fptrauth-calls" "-fptrauth-returns" "-fptrauth-auth-traps" "-fptrauth-indirect-gotos" "-faarch64-jump-table-hardening"
// CUSTOM-NOT: "-fptrauth-vtable-pointer-address-discrimination"
// CUSTOM-NOT: "-fptrauth-vtable-pointer-type-discrimination"
// CUSTOM-NOT: "-fptrauth-type-info-vtable-pointer-discrimination"
// CUSTOM-NOT: "-fptrauth-init-fini"
// CUSTOM-NOT: "-fptrauth-init-fini-address-discrimination"
// CUSTOM-NOT: "-fptrauth-function-pointer-type-discrimination"
// CUSTOM-NOT: "-fptrauth-elf-got"
// CUSTOM-NOT: "-fptrauth-proc-ind-inst-key"
// CUSTOM-NOT: "-fptrauth-proc-ind-data-key"
// CUSTOM-NOT: "-fptrauth-proc-dep-inst-key"
// CUSTOM-NOT: "-fptrauth-proc-dep-data-key"

/// The low-level -fptrauth options are available when -mpauthabi-profile=custom is used.
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=custom -mpauthabi-platform=0x0100000000000000 -mpauthabi-version=1 -fptrauth-vtable-pointer-address-discrimination -fptrauth-vtable-pointer-type-discrimination -fptrauth-type-info-vtable-pointer-discrimination -fptrauth-init-fini -fptrauth-init-fini-address-discrimination -fptrauth-function-pointer-type-discrimination -fptrauth-elf-got %s 2>&1 | FileCheck %s --check-prefix=CUSTOM-ADD
// CUSTOM-ADD: "-cc1"{{.*}} "-fptrauth-calls" "-fptrauth-returns" "-fptrauth-auth-traps" "-fptrauth-indirect-gotos" "-faarch64-jump-table-hardening"{{.*}}"-fptrauth-function-pointer-type-discrimination"
/// Use all the low-level options to toggle off the custom defaults.
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=custom -mpauthabi-platform=0x0100000000000000 -mpauthabi-version=1 -fno-ptrauth-calls -fno-ptrauth-returns -fno-ptrauth-auth-traps -fno-ptrauth-indirect-gotos -fno-aarch64-jump-table-hardening %s 2>&1 | FileCheck %s --check-prefix NONE
// NONE:     "-cc1"
// NONE-NOT: "-fptrauth-

/// The -mpauthabi=custom requires -mpauthabi-platform and -mpauthabi-platform
// RUN: not %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=custom %s 2>&1
// CUSTOM-MISSING: clang: error: custom profile must provide a platform id using '-mpauthabi-platform=<number>'
// CUSTOM-MISSING-NEXT: clang: error: custom profile must provide a non-zero version number using '-mpauthabi-version=<version>'

/// The custom profile can use the low-level options to select keys directly.
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=custom -mpauthabi-platform=0x0100000000000000 -mpauthabi-version=1 -fptrauth-proc-ind-inst-key=ib -fptrauth-proc-ind-data-key=db -fptrauth-proc-dep-inst-key=ia -fptrauth-proc-dep-data-key=da %s 2>&1 | FileCheck %s --check-prefix=CHECK-CUSTKEYS
// CHECK-CUSTKEYS: "-fptrauth-proc-ind-inst-key=ib" "-fptrauth-proc-ind-data-key=db" "-fptrauth-proc-dep-inst-key=ia" "-fptrauth-proc-dep-data-key=da"

/// -mpauthabi-profile enables -mpauthabi-platform and -mpauthabi-version only when the profile is target or custom.
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=custom -mpauthabi-platform=0x0200000000000000 -mpauthabi-version=0xff %s 2>&1 | FileCheck %s --check-prefix CHECK-CUSTVER
// CHECK-CUSTVER: "-mpauthabi-platform=0x0200000000000000" "-mpauthabi-version=0xff"

// RUN: not %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=target -mpauthabi-platform=0x0200000000000000 -mpauthabi-version=0xff %s 2>&1 | FileCheck %s --check-prefix CHECK-CUSTVERNOT
// CHECK-CUSTVERNOT: clang: error: unsupported option '-mpauthabi-platform=' for target 'aarch64-none-elf'
// CHECK-CUSTVERNOT-NEXT: clang: error: unsupported option '-mpauthabi-version=' for target 'aarch64-none-elf'
