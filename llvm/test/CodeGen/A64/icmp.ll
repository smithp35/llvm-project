; RUN: llc < %s -mtriple=a64-elf | FileCheck %s
define i64 @f(i64 %x, i64 %y)  {
entry:
  %cmp = icmp sgt i64 %x, %y
  %conv1 = zext i1 %cmp to i64
  ret i64 %conv1
}

; CHECK-LABEL: f:
; CHECK:        subs    x8, x0, x1
; CHECK-NEXT:   movz x8, #0
; CHECK-NEXT:   movz x9, #1
; CHECK-NEXT:   csel    x0, x9, x8, gt
; CHECK-NEXT:   ret
