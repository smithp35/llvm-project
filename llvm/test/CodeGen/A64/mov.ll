; RUN: llc < %s -mtriple=a64-elf | FileCheck %s

define i64 @mov0() {
  ret i64 0
}

; FIXME could use xzr register form
; CHECK-LABEL: mov0
; CHECK:       movz x0, #0
; CHECK-NEXT:  ret

define i64 @mov1() {
  ret i64 1
}

; CHECK-LABEL: mov1
; CHECK:       movz x0, #1
; CHECK-NEXT:  ret

define i64 @mov65535() {
  ret i64 65535
}

; CHECK-LABEL: mov65535
; CHECK:       movz x0, #65535
; CHECK-NEXT:  ret
