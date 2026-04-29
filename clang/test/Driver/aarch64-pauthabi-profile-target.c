// REQUIRES: aarch64-registered-target

/// Default signing-schema common to all profiles, expressed with -fptrauth options.
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=target %s 2>&1 | FileCheck %s --check-prefix COMMON
// RUN: %clang -### -c --target=aarch64-none-linux -march=armv8.3-a -mpauthabi-profile=target %s 2>&1 | FileCheck %s --check-prefix COMMON
// COMMON: "-cc1"{{.*}} "-fptrauth-intrinsics" "-fptrauth-calls" "-fptrauth-returns" "-fptrauth-auth-traps" "-fptrauth-vtable-pointer-address-discrimination" "-fptrauth-vtable-pointer-type-discrimination" "-fptrauth-type-info-vtable-pointer-discrimination" "-fptrauth-indirect-gotos" "-fptrauth-init-fini" "-fptrauth-init-fini-address-discrimination" "-faarch64-jump-table-hardening"
// COMMON-NOT: "-fptrauth-function-pointer-type-discrimination"
// COMMON-NOT: "-fptrauth-proc-ind-inst-key"
// COMMON-NOT: "-fptrauth-proc-ind-data-key"
// COMMON-NOT: "-fptrauth-proc-dep-inst-key"
// COMMON-NOT: "-fptrauth-proc-dep-data-key"

/// Bare-metal target profile signs GOT, Linux target does not.
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=target %s 2>&1 | FileCheck %s --check-prefix BARE
// RUN: %clang -### -c --target=aarch64-linux-gnu -march=armv8.3-a -mpauthabi-profile=target %s 2>&1 | FileCheck %s --check-prefix LINUX

// BARE: "-cc1"{{.*}} "-fptrauth-elf-got"
// LINUX-NOT: "-fptrauth-elf-got"

/// The low-level -fptrauth options are not available when -mpauthabi-profile=target is used. This would affect the
/// signing-schema.
// RUN: not %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=target -fptrauth-function-pointer-type-discrimination %s  2>&1 | FileCheck %s --check-prefix=ERR-LOW
// ERR-LOW: clang: error: unsupported option '-fptrauth-function-pointer-type-discrimination' for target 'aarch64-none-elf'

/// The -mpauthabi=target will set the ELF marking target and version to known values derived from driver and signing-schema defaults.
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=target %s 2>&1 | FileCheck %s --check-prefix BARE-PLAT
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=target %s 2>&1 | FileCheck %s --check-prefix BARE-VER
// BARE-PLAT: "-cc1"{{.*}} "-mpauthabi-platform=0x1"
// BARE-VER: "-cc1"{{.*}} "-mpauthabi-version=72057594037927975"
// RUN: %clang -### -c --target=aarch64-linux-gnu -march=armv8.3-a -mpauthabi-profile=target %s 2>&1 | FileCheck %s --check-prefix LINUX-PLAT
// RUN: %clang -### -c --target=aarch64-linux-gnu -march=armv8.3-a -mpauthabi-profile=target %s 2>&1 | FileCheck %s --check-prefix LINUX-VER
// LINUX-PLAT: "-cc1"{{.*}} "-mpauthabi-platform=0x2"
// LINUX-VER: "-cc1"{{.*}} "-mpauthabi-version=72057594037928487"

/// The target has additional profile selection options -mpauthabi-profile-strict and -mpauthabi-profile-relro

/// RELRO disables GOT signing. RELRO is on by default for Linux, but off for bare-metal
// RUN: %clang -### -c --target=aarch64-linux-gnu -march=armv8.3-a -mpauthabi-profile=target -mno-pauthabi-profile-relro %s 2>&1 | FileCheck %s --check-prefix COMMON
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=target -mpauthabi-profile-relro %s 2>&1 | FileCheck %s --check-prefix RELRO
// RELRO-NOT: "-fptrauth-elf-got"
// RUN: %clang -### -c --target=aarch64-linux-gnu -march=armv8.3-a -mpauthabi-profile=target -mno-pauthabi-profile-relro %s 2>&1 | FileCheck %s --check-prefix BARE
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=target -mpauthabi-profile-relro %s 2>&1 | FileCheck %s --check-prefix BARE-PLAT
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=target -mpauthabi-profile-relro %s 2>&1 | FileCheck %s --check-prefix RELRO-VER
// RELRO-VER: "-mpauthabi-version=72057594037928487"
// RUN: %clang -### -c --target=aarch64-linux-gnu -march=armv8.3-a -mpauthabi-profile=target -mno-pauthabi-profile-relro %s 2>&1 | FileCheck %s --check-prefix LINUX-PLAT
// RUN: %clang -### -c --target=aarch64-linux-gnu -march=armv8.3-a -mpauthabi-profile=target -mno-pauthabi-profile-relro %s 2>&1 | FileCheck %s --check-prefix BARE-VER

/// strict enables C function pointer type discrimination. This is off by default.
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=target -mpauthabi-profile-strict %s 2>&1 | FileCheck %s --check-prefix STRICT
// RUN: %clang -### -c --target=aarch64-linux-gnu -march=armv8.3-a -mpauthabi-profile=target -mpauthabi-profile-strict %s 2>&1 | FileCheck %s --check-prefix STRICT
// STRICT: "-cc1"{{.*}} "-fptrauth-intrinsics" "-fptrauth-calls" "-fptrauth-returns" "-fptrauth-auth-traps" "-fptrauth-vtable-pointer-address-discrimination" "-fptrauth-vtable-pointer-type-discrimination" "-fptrauth-type-info-vtable-pointer-discrimination" "-fptrauth-indirect-gotos" "-fptrauth-init-fini" "-fptrauth-init-fini-address-discrimination" "-faarch64-jump-table-hardening" "-fptrauth-function-pointer-type-discrimination"

// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=target -mpauthabi-profile-strict %s 2>&1 | FileCheck %s --check-prefix BARE-PLAT
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=target -mpauthabi-profile-strict %s 2>&1 | FileCheck %s --check-prefix STRICT-VER
// STRICT-VER: "-mpauthabi-version=72057594037928231"
// RUN: %clang -### -c --target=aarch64-linux-gnu -march=armv8.3-a -mpauthabi-profile=target -mpauthabi-profile-strict %s 2>&1 | FileCheck %s --check-prefix LINUX-PLAT
// RUN: %clang -### -c --target=aarch64-linux-gnu -march=armv8.3-a -mpauthabi-profile=target -mpauthabi-profile-strict %s 2>&1 | FileCheck %s --check-prefix RELRO-STRICT-VER
// RELRO-STRICT-VER: "-mpauthabi-version=72057594037928743"

/// Can add both strict and RELRO.
//RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=target -mpauthabi-profile-strict -mpauthabi-profile-relro %s 2>&1 | FileCheck %s --check-prefix BARE-PLAT
//RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=target -mpauthabi-profile-strict -mpauthabi-profile-relro %s 2>&1 | FileCheck %s --check-prefix RELRO-STRICT-VER

/// The bare-metal target enables an additional option to select the keys used in signing.

/// Abbreviated form is just a or b for all a key or all b key. The keys are reflected in the signing-schema
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=target -mpauthabi-profile-keys=a %s 2>&1 | FileCheck %s --check-prefix ALL-AKEY
// ALL-AKEY: "-fptrauth-proc-ind-inst-key=ia" "-fptrauth-proc-ind-data-key=da" "-fptrauth-proc-dep-inst-key=ia" "-fptrauth-proc-dep-data-key=da" "-mpauthabi-platform=0x1" "-mpauthabi-version=72057594037927970"
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=target -mpauthabi-profile-keys=b %s 2>&1 | FileCheck %s --check-prefix ALL-BKEY
// ALL-BKEY: "-fptrauth-proc-ind-inst-key=ib" "-fptrauth-proc-ind-data-key=db" "-fptrauth-proc-dep-inst-key=ib" "-fptrauth-proc-dep-data-key=db" "-mpauthabi-platform=0x1" "-mpauthabi-version=72057594037928055"

/// Long form needs all keys specified
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=target -mpauthabi-profile-keys=ibdaiadb %s 2>&1 | FileCheck %s --check-prefix=CHECK-KEYS
// CHECK-KEYS: "-fptrauth-proc-ind-inst-key=ib" "-fptrauth-proc-ind-data-key=da" "-fptrauth-proc-dep-inst-key=ia" "-fptrauth-proc-dep-data-key=db" "-mpauthabi-platform=0x1" "-mpauthabi-version=72057594037928035"

/// Check incorrect Key Syntax is diagnosed
// RUN: not %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=target -mpauthabi-profile-keys=c %s 2>&1 | FileCheck %s --check-prefix CHECK-KEYS-NOT
// RUN: not %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=target -mpauthabi-profile-keys=iada %s 2>&1 | FileCheck %s --check-prefix CHECK-KEYS-NOT
// CHECK-KEYS-NOT: clang: error: unsupported argument 'expect a, b, or iAdBiCdD where ABCD are either a or b' to option '-mpauthabi-profile-keys='
/// Correct length of string, check each key
// RUN: not %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=target -mpauthabi-profile-keys=daiadbib %s 2>&1 | FileCheck %s --check-prefix CHECK-KEY-NOT
// CHECK-KEY-NOT: clang: error: unsupported argument 'expect ia or ib for instruction key.' to option '-mpauthabi-profile-keys='
// CHECK-KEY-NOT-NEXT: clang: error: unsupported argument 'expect da or db for data key.' to option '-mpauthabi-profile-keys='
// CHECK-KEY-NOT-NEXT: clang: error: unsupported argument 'expect ia or ib for instruction key.' to option '-mpauthabi-profile-keys='
// CHECK-KEY-NOT-NEXT: clang: error: unsupported argument 'expect da or db for data key.' to option '-mpauthabi-profile-keys='

/// The linux target does not permit a choice of keys.
// RUN: not %clang -### -c --target=aarch64-linux-gnu -march=armv8.3-a -mpauthabi-profile=target -mpauthabi-profile-keys=a %s 2>&1 | FileCheck %s --check-prefix CHECK-LINUX-KEYS-NOT
// CHECK-LINUX-KEYS-NOT: clang: error: unsupported option '-mpauthabi-profile-keys=' for target 'aarch64-linux-gnu'
