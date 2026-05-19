// RUN: %clang_cc1 -E %s -triple=aarch64 | FileCheck %s
// RUN: %clang_cc1 -E %s -triple=aarch64 -fptrauth-noakey | FileCheck %s -check-prefix NOPAKEY
// RUN: %clang_cc1 -E %s -triple=aarch64 -fptrauth-nobkey | FileCheck %s -check-prefix NOPBKEY

// The -fptrauth_noakey and -fptrauth_nobkey restrict the availabilty
// of keys to the B or A family respectively. The following macros are
// used in ptrauth.h to define what key to use for the 4 roles.
__ARM_PTRAUTH_KEY_PROCESS_INDEPENDENT_CODE
__ARM_PTRAUTH_KEY_PROCESS_DEPENDENT_CODE
__ARM_PTRAUTH_KEY_PROCESS_INDEPENDENT_DATA
__ARM_PTRAUTH_KEY_PROCESS_DEPENDENT_DATA

CHECK: ptrauth_key_asia
CHECK-NEXT: ptrauth_key_asib
CHECK-NEXT: ptrauth_key_asda
CHECK-NEXT: ptrauth_key_asdb

NOPAKEY: ptrauth_key_asib
NOPAKEY-NEXT: ptrauth_key_asib
NOPAKEY-NEXT: ptrauth_key_asdb
NOPAKEY-NEXT: ptrauth_key_asdb

NOPBKEY: ptrauth_key_asia
NOPBKEY-NEXT: ptrauth_key_asia
NOPBKEY-NEXT: ptrauth_key_asda
NOPBKEY-NEXT: ptrauth_key_asda
