; RUN: llc < %s -mtriple=a64-elf | FileCheck %s
@var = dso_local global i64 0, align 8

define dso_local i64 @read() {
  %1 = load i64, i64* @var, align 8
  ret i64 %1
}

; CHECK-LABEL: read:
; CHECK:        adrp    x8, var
; CHECK-NEXT:   ldr     x0, [x8, :lo12:var]
; CHECK-NEXT:   ret
