//=- A64CallingConv.td - Calling Conventions for A64 -*- tablegen ---------*-=//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This describes the calling conventions for A64 architecture.
//
//===----------------------------------------------------------------------===//

def CC_A64 : CallingConv<[
  // Promote i8/i16/i32 arguments to i64
  CCIfType<[i8, i16, i32], CCPromoteToType<i64>>,

  // The first 8 integer arguments are passed in integer registers/
  CCIfType<[i64], CCAssignToReg<[X0, X1, X2, X3, X4, X5, X6, X7]>>,

  // Integer values get stored in stack slots that are 8-bytes in size
  // and 8-byte aligned
  CCIfType<[i1, i8, i16, i32, i64], CCAssignToStack<8,8>>]
>;

def RetCC_A64 : CallingConv<[
  // i64 are returned in registers X0, to X8
  CCIfType<[i64], CCAssignToReg<[X0, X1, X2, X3, X4, X5, X6, X7]>>,
]>;

def CC_Save : CalleeSavedRegs<(add X19, X20, X21, X22, X23, X24,
                                   X25, X26, X27, X28, LR, FP)>;
