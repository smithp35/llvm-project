; RUN: llc < %s -mtriple=a64-elf | FileCheck %s

define dso_local i64 @sub(i64 %x, i64 %y) local_unnamed_addr #0 {
entry:
  %sub = sub nsw i64 %x, %y
  ret i64 %sub
}

; CHECK-LABEL: sub:
; CHECK:        sub     x0, x0, x1
; CHECK-NEXT:   ret
