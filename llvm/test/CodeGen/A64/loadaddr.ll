; RUN: llc < %s -mtriple=a64-elf | FileCheck %s

define dso_local i64 @ld(i64* nocapture readonly %0)  {
  %2 = load i64, i64* %0, align 8
  ret i64 %2
}

; CHECK-LABEL ld:
; CHECK:        ldr     x0, [x0]
; CHECK-NEXT:   ret
