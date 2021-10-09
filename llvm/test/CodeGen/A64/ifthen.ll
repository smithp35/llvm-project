; RUN: llc < %s -mtriple=a64-elf | FileCheck %s
define dso_local i64 @select(i64 %a, i64 %b) {
entry:
  %cmp = icmp sgt i64 %a, 10
  br i1 %cmp, label %return, label %if.end

if.end:                                           ; preds = %entry
  %cmp1 = icmp sgt i64 %a, %b
  br i1 %cmp1, label %return, label %if.end3

if.end3:                                          ; preds = %if.end
  %cmp4 = icmp sgt i64 %b, 10
  %spec.select = select i1 %cmp4, i64 8, i64 0
  ret i64 %spec.select

return:                                           ; preds = %if.end, %entry
  %retval.0 = phi i64 [ 7, %entry ], [ 10, %if.end ]
  ret i64 %retval.0
}

; CHECK-LABEL: select:
; CHECK:              mov     x8, x0
; CHECK-NEXT:         subs x9, x8, #10
; CHECK-NEXT:         movz x0, #7
; CHECK-NEXT:         b.gt    .LBB0_3
; CHECK-NEXT:         b       .LBB0_1
; CHECK-LABEL: .LBB0_1:
; CHECK:              subs    x8, x8, x1
; CHECK-NEXT:         movz x0, #10
; CHECK-NEXT:         b.gt    .LBB0_3
; CHECK-NEXT:         b       .LBB0_2
; CHECK-LABEL: .LBB0_2:
; CHECK:              subs x8, x1, #10
; CHECK-NEXT:         movz x8, #0
; CHECK-NEXT:         movz x9, #1
; CHECK-NEXT:         csel    x8, x9, x8, gt
; CHECK-NEXT:         movz x9, #3
; CHECK-NEXT:         lsl     x0, x8, x9
; CHECK-LABEL: .LBB0_3:
; CHECK:              ret
