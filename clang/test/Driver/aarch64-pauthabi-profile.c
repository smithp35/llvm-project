// REQUIRES: aarch64-registered-target

// RUN: %clang -### -c --target=aarch64 %s 2>&1 | FileCheck %s --check-prefix NONE
// NONE:     "-cc1"
// NONE-NOT: "-fptrauth-

/// A -mpauthabi-profile=none is the same as no instance of -mpauthabi-profile.
// RUN: %clang -### -c --target=aarch64-linux-gnu -march=armv8.3-a -mpauthabi-profile=none %s 2>&1 | FileCheck %s --check-prefix NONE
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=none %s 2>&1 | FileCheck %s --check-prefix NONE

/// We can use a rightmost "none" profile to cancel a previous value.
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=target -mpauthabi-profile=none %s 2>&1 | FileCheck %s --check-prefix NONE

/// The -mpauthabi-profile option is not supported for anything but Linux and Bare-metal drivers
// RUN: not %clang -### -c --target=aarch64 -mpauthabi-profile=none %s 2>&1 | FileCheck %s --check-prefix ERR_UNSUP
// ERR_UNSUP: clang: error: unsupported option '-mpauthabi-profile=' for target 'aarch64'

/// The -mpauthabi-profile requires FEAT_PAUTH (armv8.3-a, or +pauth)
// RUN: not %clang -### -c --target=aarch64-linux-gnu -mpauthabi-profile=target %s 2>&1 | FileCheck %s --check-prefix ERR_UNSUPFEAT
// ERR_UNSUPFEAT: clang: error: unsupported option '-mpauthabi-profile=target' for target 'aarch64-unknown-linux-gnu'

/// Values other than none, custom or target are not supported for the prefix of mpauthabi-profile
// RUN: not %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=invalid %s 2>&1 | FileCheck %s --check-prefix ERR_PREF
// ERR_PREF: clang: error: unsupported argument 'expected target, custom or none' to option '-mpauthabi-profile='

/// The -mpauthabi-profile option other than "none" is incompatible with -mbranch-protection pac-ret, included in standard.
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=none -mbranch-protection=pac-ret %s 2>&1 | FileCheck %s --check-prefix NONE
// RUN: %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=target -mbranch-protection=bti %s 2>&1 | FileCheck %s --check-prefix BTI
// BTI: "-fptrauth-{{.*}}"-mbranch-target-enforce"
// RUN: not %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=target -mbranch-protection=standard %s 2>&1 | FileCheck %s --check-prefix ERR_STD
// ERR_STD: clang: error: the combination of '-mbranch-protection=standard' and '-mpauthabi-profile=target' is incompatible
// RUN: not %clang -### -c --target=aarch64-none-elf -march=armv8.3-a -mpauthabi-profile=target -mbranch-protection=pac-ret %s 2>&1 | FileCheck %s --check-prefix ERR_PACRET
// ERR_PACRET: clang: error: the combination of '-mbranch-protection=pac-ret' and '-mpauthabi-profile=target' is incompatible
