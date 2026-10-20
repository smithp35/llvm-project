// RUN: %clang_cc1 -E %s -triple=aarch64 | FileCheck %s
// RUN: %clang_cc1 -E -fptrauth-proc-ind-inst-key=ib -fptrauth-proc-ind-data-key=db %s -triple=aarch64 | FileCheck --check-prefix=CHECK-B %s
// RUN: %clang_cc1 -E -fptrauth-proc-dep-inst-key=ia -fptrauth-proc-dep-data-key=da %s -triple=aarch64 | FileCheck --check-prefix=CHECK-A %s
// RUN: %clang_cc1 -E -fptrauth-proc-ind-inst-key=ib -fptrauth-proc-ind-data-key=db -fptrauth-proc-dep-inst-key=ia -fptrauth-proc-dep-data-key=da %s -triple=aarch64 | FileCheck --check-prefix=CHECK-INV %s
__ARM_PTRAUTH_KEY_PROCESS_INDEPENDENT_CODE
__ARM_PTRAUTH_KEY_PROCESS_DEPENDENT_CODE
__ARM_PTRAUTH_KEY_PROCESS_INDEPENDENT_DATA
__ARM_PTRAUTH_KEY_PROCESS_DEPENDENT_DATA

CHECK: ptrauth_key_asia
CHECK-NEXT: ptrauth_key_asib
CHECK-NEXT: ptrauth_key_asda
CHECK-NEXT: ptrauth_key_asdb

CHECK-B: ptrauth_key_asib
CHECK-B-NEXT: ptrauth_key_asib
CHECK-B-NEXT: ptrauth_key_asdb
CHECK-B-NEXT: ptrauth_key_asdb

CHECK-A: ptrauth_key_asia
CHECK-A-NEXT: ptrauth_key_asia
CHECK-A-NEXT: ptrauth_key_asda
CHECK-A-NEXT: ptrauth_key_asda

CHECK-INV: ptrauth_key_asib
CHECK-INV-NEXT: ptrauth_key_asia
CHECK-INV-NEXT: ptrauth_key_asdb
CHECK-INV-NEXT: ptrauth_key_asda

