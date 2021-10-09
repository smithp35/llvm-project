; RUN: llc < %s -mtriple=a64-elf | FileCheck %s

define  i64 @mul(i64 %x, i64 %y) {
entry:
  %mul = mul nsw i64 %y, %x
  ret i64 %mul
}

; CHECK-LABEL: mul:
; CHECK:       mul x0, x1, x0
; CHECK-LABEL: ret
