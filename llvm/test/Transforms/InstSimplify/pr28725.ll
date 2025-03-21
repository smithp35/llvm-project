; NOTE: Assertions have been autogenerated by utils/update_test_checks.py
; RUN: opt -S -passes=instsimplify < %s | FileCheck %s
%S = type { i16, i32 }

; InstCombine will be able to fold this into zeroinitializer
define <2 x i16> @test1() {
; CHECK-LABEL: @test1(
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[CMP:%.*]] = icmp eq i16 extractelement (<2 x i16> bitcast (<1 x i32> splat (i32 1) to <2 x i16>), i32 0), 0
; CHECK-NEXT:    [[SEL:%.*]] = select i1 [[CMP]], [[S:%.*]] zeroinitializer, [[S]] { i16 0, i32 1 }
; CHECK-NEXT:    [[E:%.*]] = extractvalue [[S]] [[SEL]], 0
; CHECK-NEXT:    [[B:%.*]] = insertelement <2 x i16> <i16 undef, i16 0>, i16 [[E]], i32 0
; CHECK-NEXT:    ret <2 x i16> [[B]]
;
entry:
  %cmp = icmp eq i16 extractelement (<2 x i16> bitcast (<1 x i32> <i32 1> to <2 x i16>), i32 0), 0
  %sel = select i1 %cmp, %S zeroinitializer, %S { i16 0, i32 1 }
  %e = extractvalue %S %sel, 0
  %b = insertelement <2 x i16> <i16 undef, i16 0>, i16 %e, i32 0
  ret <2 x i16> %b
}
