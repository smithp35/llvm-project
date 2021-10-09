; RUN: llc < %s -mtriple=a64-elf | FileCheck %s

; Simplest possible codegen test for lower return
define void @ret() {
entry:
  ret void
}

; CHECK-LABEL: ret:
; CHECK: ret

; return a value, lower formal parameters and return value
define i64 @ret_zero(i64 %x) {
entry:
  ret i64 %x
}

; CHECK-LABEL: ret_zero:
; CHECK: ret
