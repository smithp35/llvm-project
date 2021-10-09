; RUN: llc < %s -mtriple=a64-elf | FileCheck %s

define  i64 @add(i64 %x, i64 %y) {
entry:
  %add = add nsw i64 %y, %x
  ret i64 %add
}

; CHECK-LABEL: add:
; CHECK:       add     x0, x1, x0
; CHECK-NEXT:  ret

define i64 @add2(i64 %x) {
entry:
   %add2 = add nsw i64 %x, 4
   ret i64 %add2
}

; CHECK-LABEL: add2:
; CHECK:       add x0, x0, #4
; CHECK-LABEL: ret

define i64 @addshift(i64 %x, i64 %y) {
entry:
  %shl = shl i64 %y, 4
  %add = add nsw i64 %shl, %x
  ret i64 %add
}

; CHECK-LABEL:  addshift:
; CHECK: 	add x0, x0, x1, lsl #4
; CHECK-NEXT: 	ret
