; RUN: llc < %s -mtriple=a64-elf | FileCheck %s
define void @func() {
entry:
  ret void
}

; CHECK-LABEL: func:
; CHECK:        ret

define dso_local void @call() {
entry:
  call void @func()
  ret void
}

; CHECK-LABEL: call:
; CHECK:       sub sp, sp, #16
; CHECK-NEXT:  str     x30, [sp, #8]
; CHECK-NEXT:  bl      func
; CHECK-NEXT:  ldr     x30, [sp, #8]
; CHECK-NEXT:  add sp, sp, #16
; CHECK-NEXT:  ret

define dso_local i64 @func2(i64 %a) {
entry:
  ret i64 %a
}

; CHECK-LABEL: func2:
; CHECK:       ret

define dso_local i64 @call2() {
entry:
  %call = call i64 @func2(i64 2)
  ret i64 %call
}

; CHECK-LABEL: call2:
; CHECK:       sub sp, sp, #16
; CHECK-NEXT:  str     x30, [sp, #8]
; CHECK-NEXT:  movz x0, #2
; CHECK-NEXT:  bl      func2
; CHECK-NEXT:  ldr     x30, [sp, #8]
; CHECK-NEXT:  add sp, sp, #16
; CHECK-NEXT:  ret
