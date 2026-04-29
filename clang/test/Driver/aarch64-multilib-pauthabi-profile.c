// RUN: %clang --target=aarch64-none-elf -mpauthabi-profile=target -march=armv8.3-a -print-multi-flags-experimental -c %s | FileCheck --check-prefixes=TARGET %s
// TARGET: -mpauthabi-profile=target
// TARGET-NOT: -mpauthabi-profile-strict
// TARGET-NOT :-mpauthabi-profile-relro
// TARGET-NOT :-mpauthabi-profile-keys

// RUN: %clang --target=aarch64-none-elf -mpauthabi-profile=target -mpauthabi-profile-strict -march=armv8.3-a -print-multi-flags-experimental -c %s | FileCheck --check-prefixes=STRICT %s
// STRICT: -mpauthabi-profile-strict
// STRICT: -mpauthabi-profile=target
// STRICT-NOT: -mpauthabi-profile-relro
// STRICT-NOT: -mpauthabi-profile-keys

// RUN: %clang --target=aarch64-none-elf -mpauthabi-profile=target -mpauthabi-profile-relro -march=armv8.3-a -print-multi-flags-experimental -c %s | FileCheck --check-prefixes=RELRO %s
// RELRO: -mpauthabi-profile-relro
// RELRO: -mpauthabi-profile=target
// RELRO-NOT: -mpauthabi-profile-strict
// RELRO-NOT: -mpauthabi-profile-keys

// RUN: %clang --target=aarch64-none-elf -mpauthabi-profile=target -mpauthabi-profile-strict -mpauthabi-profile-relro -march=armv8.3-a -print-multi-flags-experimental -c %s | FileCheck --check-prefixes=STRICTRELRO %s
// STRICTRELRO: -mpauthabi-profile-relro
// STRICTRELRO: -mpauthabi-profile-strict
// STRICTRELRO: -mpauthabi-profile=target
// STRICTRELRO-NOT: -mpauthabi-profile-keys

// RUN: %clang --target=aarch64-none-elf -mpauthabi-profile=target -mpauthabi-profile-keys=a -march=armv8.3-a -print-multi-flags-experimental -c %s | FileCheck --check-prefixes=KEYA %s
// RUN: %clang --target=aarch64-none-elf -mpauthabi-profile=target -mpauthabi-profile-keys=iadaiada -march=armv8.3-a -print-multi-flags-experimental -c %s | FileCheck --check-prefixes=KEYA %s

// KEYA:-mpauthabi-profile-keys=iadaiada
// KEYA: -mpauthabi-profile=target
// KEYA-NOT: -mpauthabi-profile-strict
// KEYA-NOT :-mpauthabi-profile-relro

// RUN: %clang --target=aarch64-none-elf -mpauthabi-profile=target -mpauthabi-profile-keys=b -march=armv8.3-a -print-multi-flags-experimental -c %s | FileCheck --check-prefixes=KEYB %s
// RUN: %clang --target=aarch64-none-elf -mpauthabi-profile=target -mpauthabi-profile-keys=ibdbibdb -march=armv8.3-a -print-multi-flags-experimental -c %s | FileCheck --check-prefixes=KEYB %s
// KEYB:-mpauthabi-profile-keys=ibdbibdb
// KEYB: -mpauthabi-profile=target
// KEYB-NOT: -mpauthabi-profile-strict
// KEYB-NOT :-mpauthabi-profile-relro

// RUN: %clang --target=aarch64-none-elf -mpauthabi-profile=target -mpauthabi-profile-strict -mpauthabi-profile-relro -mpauthabi-profile-keys=iadbibda -march=armv8.3-a -print-multi-flags-experimental -c %s
// ALL: -mpauthabi-profile-keys=iadbibda
// ALL: -mpauthabi-profile-relro
// ALL: -mpauthabi-profile-strict
// ALL: -mpauthabi-profile=target
