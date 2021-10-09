; RUN: llc < %s -mtriple=a64-elf | FileCheck %s
define i64 @factorial(i64 %x) {
entry:
  %cmp = icmp eq i64 %x, 0
  br i1 %cmp, label %return, label %if.end

if.end:                                           ; preds = %entry
  %sub = add nsw i64 %x, -1
  %call = call i64 @factorial(i64 %sub)
  %mul = mul nsw i64 %call, %x
  br label %return

return:                                           ; preds = %entry, %if.end
  %retval.0 = phi i64 [ %mul, %if.end ], [ 1, %entry ]
  ret i64 %retval.0
}

; CHECK-LABEL: factorial:
; CHECK:              sub sp, sp, #16
; CHECK-NEXT:         str     x19, [sp, #8]
; CHECK-NEXT:         str     x30, [sp]
; CHECK-NEXT:         mov     x19, x0
; CHECK-NEXT:         subs x8, x19, #0
; CHECK-NEXT:         movz x0, #1
; CHECK-NEXT:         b.eq    .LBB0_2
; CHECK-NEXT:         b       .LBB0_1
; CHECK-LABEL: .LBB0_1:
; CHECK:              movn x8, #0
; CHECK-NEXT:         add     x0, x19, x8
; CHECK-NEXT:         bl      factorial
; CHECK-NEXT:         mul x0, x0, x19
; CHECK-LABEL: .LBB0_2:
; CHECK:              ldr     x30, [sp]
; CHECK-NEXT:         ldr     x19, [sp, #8]
; CHECK-NEXT:         add sp, sp, #16
; CHECK-NEXT:         ret
