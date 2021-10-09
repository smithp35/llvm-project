; RUN: llc < %s -mtriple=a64-elf | FileCheck %s

@var = dso_local global i64 0, align 8

define dso_local nonnull i64* @read() {
  ret i64* @var
}

; CHECK-LABEL: read:
; CHECK:        adrp    x0, var
; CHECK-NEXT:   add x0, x0, :lo12:var
; CHECK-NEXT:   ret
