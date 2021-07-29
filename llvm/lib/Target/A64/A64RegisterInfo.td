//=- A64RegisterInfo.td - Describe the A64 Registers -*- tablegen         -*-=//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//

class A64Reg<bits<16> enc, string n,
            list<string> altNames = []>
         : Register<n, altNames> {
  let HWEncoding = enc;
  let Namespace = "A64";
}

//===----------------------------------------------------------------------===//
// Registers
//===----------------------------------------------------------------------===//

def X0    : A64Reg<0,   "x0" >, DwarfRegNum<[0]>;
def X1    : A64Reg<1,   "x1" >, DwarfRegNum<[1]>;
def X2    : A64Reg<2,   "x2" >, DwarfRegNum<[2]>;
def X3    : A64Reg<3,   "x3" >, DwarfRegNum<[3]>;
def X4    : A64Reg<4,   "x4" >, DwarfRegNum<[4]>;
def X5    : A64Reg<5,   "x5" >, DwarfRegNum<[5]>;
def X6    : A64Reg<6,   "x6" >, DwarfRegNum<[6]>;
def X7    : A64Reg<7,   "x7" >, DwarfRegNum<[7]>;
def X8    : A64Reg<8,   "x8" >, DwarfRegNum<[8]>;
def X9    : A64Reg<9,   "x9" >, DwarfRegNum<[9]>;
def X10   : A64Reg<10, "x10">, DwarfRegNum<[10]>;
def X11   : A64Reg<11, "x11">, DwarfRegNum<[11]>;
def X12   : A64Reg<12, "x12">, DwarfRegNum<[12]>;
def X13   : A64Reg<13, "x13">, DwarfRegNum<[13]>;
def X14   : A64Reg<14, "x14">, DwarfRegNum<[14]>;
def X15   : A64Reg<15, "x15">, DwarfRegNum<[15]>;
def X16   : A64Reg<16, "x16">, DwarfRegNum<[16]>;
def X17   : A64Reg<17, "x17">, DwarfRegNum<[17]>;
def X18   : A64Reg<18, "x18">, DwarfRegNum<[18]>;
def X19   : A64Reg<19, "x19">, DwarfRegNum<[19]>;
def X20   : A64Reg<20, "x20">, DwarfRegNum<[20]>;
def X21   : A64Reg<21, "x21">, DwarfRegNum<[21]>;
def X22   : A64Reg<22, "x22">, DwarfRegNum<[22]>;
def X23   : A64Reg<23, "x23">, DwarfRegNum<[23]>;
def X24   : A64Reg<24, "x24">, DwarfRegNum<[24]>;
def X25   : A64Reg<25, "x25">, DwarfRegNum<[25]>;
def X26   : A64Reg<26, "x26">, DwarfRegNum<[26]>;
def X27   : A64Reg<27, "x27">, DwarfRegNum<[27]>;
def X28   : A64Reg<28, "x28">, DwarfRegNum<[28]>;
def FP    : A64Reg<29, "x29">, DwarfRegNum<[29]>;
def LR    : A64Reg<30, "x30">, DwarfRegNum<[30]>;
def SP    : A64Reg<31, "sp">,  DwarfRegNum<[31]>;
def XZR   : A64Reg<31, "xzr">, DwarfRegAlias<SP>;

// Condition code register.
def NZCV  : A64Reg<0, "nzcv">;

// GPR register classes with the intersections of GPR32/GPR32sp and
// GPR64/GPR64sp for use by the coalescer.
def GPR64common : RegisterClass<"A64", [i64], 64,
                                (add (sequence "X%u", 0, 28), FP, LR)> {
  let AltOrders = [(rotl GPR64common, 8)];
  let AltOrderSelect = [{ return 1; }];
}
def GPR64 : RegisterClass<"A64", [i64], 64, (add GPR64common, XZR)> {
  let AltOrders = [(rotl GPR64, 8)];
  let AltOrderSelect = [{ return 1; }];
}

// GPR register classes which include SP.
def GPR64sp : RegisterClass<"A64", [i64], 64, (add GPR64common, SP)> {
  let AltOrders = [(rotl GPR64sp, 8)];
  let AltOrderSelect = [{ return 1; }];
}

def GPR64sponly : RegisterClass<"A64", [i64], 64, (add SP)>;

// Condition code regclass.
def CCR : RegisterClass<"A64", [i32], 32, (add NZCV)> {
  let CopyCost = -1;  // Don't allow copying of status registers.

  // CCR is not allocatable.
  let isAllocatable = 0;
}

def GPR64all : RegisterClass<"A64", [i64], 64, (add GPR64common, XZR, SP)>;

