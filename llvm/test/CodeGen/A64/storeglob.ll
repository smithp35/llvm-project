; RUN: llc < %s -mtriple=a64-elf | FileCheck %s

@var = dso_local global i64 0, align 8

define dso_local void @write(i64 %0) {
  store i64 %0, i64* @var, align 8
  ret void
}

; CHECK-LABEL: write:
; CHECK:        adrp    x8, var
; CHECK-NEXT:   str     x0, [x8, :lo12:var]
; CHECK-NEXT:   ret
