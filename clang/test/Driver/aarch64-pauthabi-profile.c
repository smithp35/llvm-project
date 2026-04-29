// REQUIRES: aarch64-registered-target

// RUN: %clang -### -c --target=aarch64 %s 2>&1 | FileCheck %s --check-prefix NONE
// NONE:     "-cc1"
// NONE-NOT: "-fptrauth-

//// A -mpauthabi-profile=none is the same as no instance of -mpauthabi-profile.
// RUN: %clang -### -c --target=aarch64-linux-gnu -march=armv8.3-a -mpauthabi-profile=none %s 2>&1 | FileCheck %s --check-prefix NONE
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=none %s 2>&1 | FileCheck %s --check-prefix NONE

//// We can use a rightmost "none" profile to cancel a previous value.
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=platform -mpauthabi-profile=none %s 2>&1 | FileCheck %s --check-prefix NONE


//// The -mpauthabi-profile option is not supported for anything but Linux and Bare-metal drivers
// RUN: not %clang -### -c --target=aarch64 -mpauthabi-profile=none %s 2>&1 | FileCheck %s --check-prefix ERR_UNSUP
// ERR_UNSUP: clang: error: unsupported option '-mpauthabi-profile=' for target 'aarch64'

//// The -mpauthabi-profile requires FEAT_PAUTH (armv8.3-a, or +pauth)
// RUN: not %clang -### -c --target=aarch64-linux-gnu -mpauthabi-profile=platform %s 2>&1 | FileCheck %s --check-prefix ERR_UNSUPFEAT
// ERR_UNSUPFEAT: clang: error: unsupported option '-mpauthabi-profile=platform' for target 'aarch64-unknown-linux-gnu'

//// Default signing-schema common to all profiles, expressed with -fptrauth options.
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=platform %s 2>&1 | FileCheck %s --check-prefix COMMON
// RUN: %clang -### -c --target=aarch64-none-linux -march=armv8.3-a -mpauthabi-profile=platform %s 2>&1 | FileCheck %s --check-prefix COMMON
// COMMON: "-cc1"{{.*}} "-fptrauth-intrinsics" "-fptrauth-calls" "-fptrauth-returns" "-fptrauth-auth-traps" "-fptrauth-vtable-pointer-address-discrimination" "-fptrauth-vtable-pointer-type-discrimination" "-fptrauth-type-info-vtable-pointer-discrimination" "-fptrauth-indirect-gotos" "-fptrauth-init-fini" "-fptrauth-init-fini-address-discrimination" "-faarch64-jump-table-hardening" {{.*}}
// COMMON-NOT: "-fptrauth-function-pointer-type-discrimination"
// COMMON-NOT: "-fptrauth-noakey"
// COMMON-NOT: "-fptrauth-nobkey"

//// Bare-metal platform profile signs GOT, Linux platform does not.
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=platform %s 2>&1 | FileCheck %s --check-prefix BARE
// RUN: %clang -### -c --target=aarch64-linux-gnu -march=armv8.3-a -mpauthabi-profile=platform %s 2>&1 | FileCheck %s --check-prefix LINUX

// BARE: "-cc1"{{.*}} "-fptrauth-elf-got" {{.*}}
// LINUX-NOT: "-fptrauth-elf-got"

//// Custom profile defaults to the minimum viable signing schema
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=custom %s 2>&1 | FileCheck %s --check-prefix CUSTOM
// RUN: %clang -### -c --target=aarch64-linux-gnu -march=armv8.3-a -mpauthabi-profile=custom %s 2>&1 | FileCheck %s --check-prefix CUSTOM
// CUSTOM: "-cc1"{{.*}} "-fptrauth-calls" "-fptrauth-returns" "-fptrauth-auth-traps" "-fptrauth-indirect-gotos" "-faarch64-jump-table-hardening" {{.*}}
// CUSTOM-NOT: "-fptrauth-vtable-pointer-address-discrimination"
// CUSTOM-NOT: "-fptrauth-vtable-pointer-type-discrimination"
// CUSTOM-NOT: "-fptrauth-type-info-vtable-pointer-discrimination"
// CUSTOM-NOT: "-fptrauth-init-fini"
// CUSTOM-NOT: "-fptrauth-init-fini-address-discrimination"
// CUSTOM-NOT: "-fptrauth-function-pointer-type-discrimination"
// CUSTOM-NOT: "-fptrauth-elf-got"
// CUSTOM-NOT: "-fptrauth-noakey"
// CUSTOM-NOT: "-fptrauth-nobkey"

//// Values other than none, custom or platform are not supported for the prefix of mpauthabi-profile
// RUN: not %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=invalid %s 2>&1 | FileCheck %s --check-prefix ERR_PREF
// ERR_PREF: clang: error: unsupported argument 'expected platform, custom or none' to option '-mpauthabi-profile='

//// The low-level -fptrauth options are not available when -mpauthabi-profile=platform is used. This would affect the
//// signing-schema.
// RUN: not %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=platform -fptrauth-function-pointer-type-discrimination %s  2>&1 | FileCheck %s --check-prefix=ERR-LOW
// ERR-LOW: clang: error: unsupported option '-fptrauth-function-pointer-type-discrimination' for target 'aarch64-none-elf'

//// The low-level -fptrauth options are available when -mpauthabi-profile=custom is used.
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=custom -fptrauth-function-pointer-type-discrimination %s 2>&1 | FileCheck %s --check-prefix=CUSTOM-ADD
// CUSTOM-ADD: "-cc1"{{.*}} "-fptrauth-calls" "-fptrauth-returns" "-fptrauth-auth-traps" "-fptrauth-indirect-gotos" "-faarch64-jump-table-hardening" {{.*}} "-fptrauth-function-pointer-type-discrimination"{{.*}}
//// Use all the low-level options to toggle off the custom defaults.
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=custom -fno-ptrauth-calls -fno-ptrauth-returns -fno-ptrauth-auth-traps -fno-ptrauth-indirect-gotos -fno-aarch64-jump-table-hardening %s 2>&1 | FileCheck %s --check-prefix NONE

//// The -mpauthabi=platform will set the ELF marking platform and version to known values derived from driver and signing-schema defaults.
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=platform %s 2>&1 | FileCheck %s --check-prefix BARE-PLAT
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=platform %s 2>&1 | FileCheck %s --check-prefix BARE-VER
// BARE-PLAT: "-cc1"{{.*}} "-mpauthabi-platform=0x1"{{.*}}
// BARE-VER: "-cc1"{{.*}} "-mpauthabi-version=72057594037927939"{{.*}}
// RUN: %clang -### -c --target=aarch64-linux-gnu -march=armv8.3-a -mpauthabi-profile=platform %s 2>&1 | FileCheck %s --check-prefix LINUX-PLAT
// RUN: %clang -### -c --target=aarch64-linux-gnu -march=armv8.3-a -mpauthabi-profile=platform %s 2>&1 | FileCheck %s --check-prefix LINUX-VER
// LINUX-PLAT: "-cc1"{{.*}} "-mpauthabi-platform=0x2"{{.*}}
// LINUX-VER: "-cc1"{{.*}} "-mpauthabi-version=72057594037927947"{{.*}}

//// The -mpauthabi=custom has a default platform in the reserved space, but does not derive a version in the Driver.
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=custom %s 2>&1 | FileCheck %s --check-prefix CUSTOM-PV
// CUSTOM-PV: "-cc1"{{.*}}"-mpauthabi-platform=0x0100000000000000"{{.*}}
// CUSTOM-PV-NOT: -mpauthabi-version

//// The platform has additional profile selection options that can be toggled with +option and +nooption.
//// These affect the signing schema, and affect the version, but stay within the same platform.

//// RELRO disables GOT signing. RELRO is on by default for Linux, but off for bare-metal
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=platform+relro %s 2>&1 | FileCheck %s --check-prefix COMMON
// RUN: %clang -### -c --target=aarch64-linux-gnu -march=armv8.3-a -mpauthabi-profile=platform+norelro %s 2>&1 | FileCheck %s --check-prefix COMMON
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=platform+relro %s 2>&1 | FileCheck %s --check-prefix RELRO
// RELRO-NOT: "-fptrauth-elf-got"
// RUN: %clang -### -c --target=aarch64-linux-gnu -march=armv8.3-a -mpauthabi-profile=platform+norelro %s 2>&1 | FileCheck %s --check-prefix BARE
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=platform+relro %s 2>&1 | FileCheck %s --check-prefix BARE-PLAT
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=platform+relro %s 2>&1 | FileCheck %s --check-prefix RELRO-VER
// RELRO-VER: "-mpauthabi-version=72057594037927947"
// RUN: %clang -### -c --target=aarch64-linux-gnu -march=armv8.3-a -mpauthabi-profile=platform+norelro %s 2>&1 | FileCheck %s --check-prefix LINUX-PLAT
// RUN: %clang -### -c --target=aarch64-linux-gnu -march=armv8.3-a -mpauthabi-profile=platform+norelro %s 2>&1 | FileCheck %s --check-prefix BARE-VER

//// strict enables C function pointer type discrimination. This is off by default.
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=platform+strict %s 2>&1 | FileCheck %s --check-prefix COMMON
// RUN: %clang -### -c --target=aarch64-linux-gnu -march=armv8.3-a -mpauthabi-profile=platform+strict %s 2>&1 | FileCheck %s --check-prefix COMMON
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=platform+strict %s 2>&1 | FileCheck %s --check-prefix BARE-PLAT
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=platform+strict %s 2>&1 | FileCheck %s --check-prefix STRICT-VER
// STRICT-VER: "-mpauthabi-version=72057594037927943"
// RUN: %clang -### -c --target=aarch64-linux-gnu -march=armv8.3-a -mpauthabi-profile=platform+strict %s 2>&1 | FileCheck %s --check-prefix LINUX-PLAT
// RUN: %clang -### -c --target=aarch64-linux-gnu -march=armv8.3-a -mpauthabi-profile=platform+strict %s 2>&1 | FileCheck %s --check-prefix RELRO-STRICT-VER
// RELRO-STRICT-VER: "-mpauthabi-version=72057594037927951"

//// Can add both strict and RELRO.
//RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=platform+strict+relro %s 2>&1 | FileCheck %s --check-prefix BARE-PLAT
//RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=platform+strict+relro %s 2>&1 | FileCheck %s --check-prefix RELRO-STRICT-VER

//// Version can be added to the profile, only the default version 1 is available at the moment.
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=platform+v1 %s 2>&1 | FileCheck %s --check-prefix BARE-PLAT
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=platform+v1 %s 2>&1 | FileCheck %s --check-prefix BARE-VER

//// Diagnose out of range version
// RUN: not %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=platform+v2 %s 2>&1 | FileCheck %s --check-prefix UNSUPPORTED-VER
// UNSUPPORTED-VER: clang: error: unsupported argument 'Unknown profile version for platfom' to option '-mpauthabi-profile='
// RUN: not %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=platform+v256 %s 2>&1 | FileCheck %s --check-prefix OUT-OF-RANGE-VER
// OUT-OF-RANGE-VER: clang: error: unsupported argument 'out of range version number, must be in range [0-255]' to option '-mpauthabi-profile='

//// The bare-metal platform permits one of the key families (A, B) to be disabled.
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=platform+noakey %s 2>&1 | FileCheck %s --check-prefix NOAKEY
// NOAKEY: "-fptrauth-noakey"
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=platform+noakey %s 2>&1 | FileCheck %s --check-prefix NOAKEY-VER
// NOAKEY-VER: "-mpauthabi-version=72057594037927937"
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=platform+nobkey %s 2>&1 | FileCheck %s --check-prefix NOBKEY
// NOBKEY: "-fptrauth-nobkey"
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=platform+nobkey %s 2>&1 | FileCheck %s --check-prefix NOBKEY-VER
// NOBKEY-VER: "-mpauthabi-version=72057594037927938"


//// The linux platform does not permit a choice of keys.
// not %clang -### -c --target=aarch64-linux-gnu -march=armv8.3-a -mpauthabi-profile=platform+noakey %s 2>&1 | FileCheck %s --check-prefix ERR-LINUX-KEYS
// not %clang -### -c --target=aarch64-linux-gnu -march=armv8.3-a -mpauthabi-profile=platform+nobkey %s 2>&1 | FileCheck %s --check-prefix ERR-LINUX-KEYS
// ERR-LINUX-KEYS: clang: error: unsupported argument 'Linux platform requires both A and B keys' to option '-mpauthabi-profile='

//// The custom profile can use the low-level options directly.
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=custom -fptrauth-noakey %s 2>&1 | FileCheck %s --check-prefix NOAKEY
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=custom -fptrauth-nobkey %s 2>&1 | FileCheck %s --check-prefix NOBKEY

//// Check that both keys haven't been disabled.
// RUN: not %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=custom -fptrauth-noakey -fptrauth-nobkey %s 2>&1 | FileCheck %s --check-prefix CHECK-FPTRKEYS
// CHECK-FPTRKEYS: clang: error: the combination of '-fptrauth_noakey' and '-fptrauth_nobkey' is incompatible
// RUN: not %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=platform+noakey+nobkey %s 2>&1 | FileCheck %s --check-prefix CHECK-PROFKEYS
// CHECK-PROFKEYS: unsupported argument 'At least one of A or B key must be enabled' to option '-mpauthabi-profile='

//// -mpauthabi-profile enables -mpauthabi-platform and -mpauthabi-version when the profile is platform or custom.
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=platform -mpauthabi-platform=0x0200000000000000 -mpauthabi-version=0xff %s 2>&1 | FileCheck %s --check-prefix CHECK-PLATVER
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=custom -mpauthabi-platform=0x0200000000000000 -mpauthabi-version=0xff %s 2>&1 | FileCheck %s --check-prefix CHECK-PLATVER
// CHECK-PLATVER: "-mpauthabi-platform=0x0200000000000000" "-mpauthabi-version=0xff"
