//=- A64InstrInfo.td - Describe the A64 Instructions     -*- tablegen -----*-=//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// A64 Instruction definitions.
//
//===----------------------------------------------------------------------===//

def A64callseq_start : SDNode<"ISD::CALLSEQ_START",
                            SDCallSeqStart<[ SDTCisVT<0, i32>,
                                             SDTCisVT<1, i32> ]>,
                            [SDNPHasChain, SDNPOutGlue]>;
def A64callseq_end   : SDNode<"ISD::CALLSEQ_END",
                            SDCallSeqEnd<[ SDTCisVT<0, i32>,
                                           SDTCisVT<1, i32> ]>,
                            [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;

def SDT_A64CCMP : SDTypeProfile<1, 5,
                                [SDTCisVT<0, i32>,
                                 SDTCisInt<1>,
                                 SDTCisSameAs<1, 2>,
                                 SDTCisInt<3>,
                                 SDTCisInt<4>,
                                 SDTCisVT<5, i32>]>;

def SDT_A64Brcond  : SDTypeProfile<0, 3,
                                   [SDTCisVT<0, OtherVT>, SDTCisVT<1, i64>,
                                    SDTCisVT<2, i32>]>;

def SDT_A64CSel  : SDTypeProfile<1, 4,
                                 [SDTCisSameAs<0, 1>,
                                  SDTCisSameAs<0, 2>,
                                  SDTCisInt<3>,
                                  SDTCisVT<4, i32>]>;

// SDTBinaryArithWithFlagsOut - RES1, FLAGS = op LHS, RHS
def SDTBinaryArithWithFlagsOut : SDTypeProfile<2, 2,
                                              [SDTCisSameAs<0, 2>,
                                               SDTCisSameAs<0, 3>,
                                               SDTCisInt<0>, SDTCisVT<1, i32>]>;

// Target Specific Nodes
def A64retflag       : SDNode<"A64ISD::RET_FLAG", SDTNone,
                            [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;
def A64call          : SDNode<"A64ISD::CALL",
                            SDTypeProfile<0, -1, [SDTCisPtrTy<0>]>,
                            [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue,
                             SDNPVariadic]>;
def A64adrp          : SDNode<"A64ISD::ADRP", SDTIntUnaryOp, []>;
def A64addlow        : SDNode<"A64ISD::ADDlow", SDTIntBinOp, []>;
def A64ccmp          : SDNode<"A64ISD::CCMP",  SDT_A64CCMP>;
def A64ccmn          : SDNode<"A64ISD::CCMN",  SDT_A64CCMP>;
def A64brcond        : SDNode<"A64ISD::BRCOND", SDT_A64Brcond,
                                [SDNPHasChain]>;
def A64add_flag  : SDNode<"A64ISD::ADDS",  SDTBinaryArithWithFlagsOut,
                            [SDNPCommutative]>;
def A64sub_flag  : SDNode<"A64ISD::SUBS",  SDTBinaryArithWithFlagsOut>;
def A64csel  : SDNode<"A64ISD::CSEL", SDT_A64CSel>;

include "A64InstrFormats.td"

let Defs = [SP], Uses = [SP], hasSideEffects = 1, isCodeGenOnly = 1 in {
def ADJCALLSTACKDOWN : Pseudo<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                              [(A64callseq_start timm:$amt1, timm:$amt2)]>,
                               Sched<[]>;
def ADJCALLSTACKUP : Pseudo<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                            [(A64callseq_end timm:$amt1, timm:$amt2)]>,
                             Sched<[]>;
}
// Comment

//===----------------------------------------------------------------------===//
// Hint Instruction
//===----------------------------------------------------------------------===//
def NOP : Hint<0b0000, 0b000, (outs), (ins), "nop", []> {}

//===----------------------------------------------------------------------===//
// Operands
//===----------------------------------------------------------------------===//
def uimm12 : Operand<i32>, ImmLeaf<i32, [{
    return Imm >= 0 && Imm < 4096;
}]>;

//===----------------------------------------------------------------------===//
// Data processing instructions in long form. To be factored later
//===----------------------------------------------------------------------===//

//def MOVWi : DataProcessingImm<0b101, (outs GPR$rd), (ins simm16$imm16),
//                              "mov", "$rd, $imm16">, Sched<[]>

/// Arm ARM Add/subtract (immediate)

// Non flag setting
defm ADDi : AddSub<"add", 0>;
defm SUBi : AddSub<"sub", 1>;

def : Pat<(add GPR64:$src1, addsub_shifted_imm64:$src2),
          (ADDiXri GPR64:$src1, addsub_shifted_imm64:$src2)>;
def : Pat<(sub GPR64:$src1, addsub_shifted_imm64:$src2),
          (SUBiXri GPR64:$src1, addsub_shifted_imm64:$src2)>;

// Flag setting
defm ADDSi : AddSubS<"adds", 0>;
defm SUBSi : AddSubS<"subs", 1>;

def : Pat<(A64add_flag GPR64:$src1, addsub_shifted_imm64:$src2),
          (ADDSiXri GPR64:$src1, addsub_shifted_imm64:$src2)>;
def : Pat<(A64sub_flag GPR64:$src1, addsub_shifted_imm64:$src2),
          (SUBSiXri GPR64:$src1, addsub_shifted_imm64:$src2)>;

def : InstAlias<"cmp $src, $imm", (SUBSiXri XZR, GPR64sp:$src,
                                   addsub_shifted_imm64:$imm)>;
def : InstAlias<"cmn $src, $imm", (ADDSiXri XZR, GPR64sp:$src,
                                   addsub_shifted_imm64:$imm)>;

defm ADD : AddSubRegGen2<"add", 0>;
defm SUB : AddSubRegGen2<"sub", 1>;
defm ADDS : AddSubRegGen2S<"adds", 0>;
defm SUBS : AddSubRegGen2S<"subs", 1>;

def : InstAlias<"cmp $src, $src2$sh", (SUBSXrs XZR, GPR64:$src,
                GPR64:$src2, arith_shift64:$sh), 5>;
def : InstAlias<"cmn $src, $src2$sh", (ADDSXrs XZR, GPR64:$src,
                GPR64:$src2, arith_shift64:$sh), 5>;

def : Pat<(add GPR64:$src1, arith_shifted_reg64:$src2),
          (ADDXrs GPR64:$src1, arith_shifted_reg64:$src2)>;
def : Pat<(sub GPR64:$src1, arith_shifted_reg64:$src2),
          (SUBXrs GPR64:$src1, arith_shifted_reg64:$src2)>;

def : Pat<(A64add_flag GPR64:$src1, arith_shifted_reg64:$src2),
          (ADDSXrs GPR64:$src1, arith_shifted_reg64:$src2)>;
def : Pat<(A64sub_flag GPR64:$src1, arith_shifted_reg64:$src2),
          (SUBSXrs GPR64:$src1, arith_shifted_reg64:$src2)>;

def ADDXrr : Pseudo<(outs GPR64:$rd), (ins GPR64:$rn, GPR64:$rm),
                    [(set GPR64:$rd, (add GPR64:$rn, GPR64:$rm))]>;
def SUBXrr : Pseudo<(outs GPR64:$rd), (ins GPR64:$rn, GPR64:$rm),
                    [(set GPR64:$rd, (sub GPR64:$rn, GPR64:$rm))]>;
let Defs = [NZCV], isCompare = 1 in {
def ADDSXrr : Pseudo<(outs GPR64:$rd), (ins GPR64:$rn, GPR64:$rm),
                    [(set GPR64:$rd, (A64add_flag GPR64:$rn, GPR64:$rm))]>;
def SUBSXrr : Pseudo<(outs GPR64:$rd), (ins GPR64:$rn, GPR64:$rm),
                    [(set GPR64:$rd, (A64sub_flag GPR64:$rn, GPR64:$rm))]>;
}

/// Arm ARM Logical (immediates)
defm AND : LogicalImm<"and", 0b00, "bic">;
defm ORR : LogicalImm<"orr", 0b01, "orn">;
defm EOR : LogicalImm<"eor", 0b10, "eon">;
defm ANDS : LogicalImmS<"ands", 0b11, "bics">;

def : Pat<(and GPR64:$src1, logical_imm64:$src2),
          (ANDXri GPR64:$src1, logical_imm64:$src2)>;
def : Pat<(or GPR64:$src1,  logical_imm64:$src2),
          (ORRXri GPR64:$src1, logical_imm64:$src2)>;
def : Pat<(xor GPR64:$src1,  logical_imm64:$src2),
          (EORXri GPR64:$src1, logical_imm64:$src2)>;

/// Aliases for MOV (architectural)
def : InstAlias<"mov $dst, $imm", (ORRXri GPR64sp:$dst, XZR, logical_imm64:$imm), 0>;

/// Arm ARM Logical (shifted register)

// Non flag setting

defm ANDr : LogicalRegGen<"and", 0b00, 0b0>;
defm BICr : LogicalRegGen<"bic", 0b00, 0b1>;
defm ORRr : LogicalRegGen<"orr", 0b01, 0b0>;
defm ORNr : LogicalRegGen<"orn", 0b01, 0b1>;
defm EORr : LogicalRegGen<"eor", 0b10, 0b0>;
defm EONr : LogicalRegGen<"eon", 0b10, 0b1>;

def : Pat<(and GPR64:$src1, logical_shifted_reg64:$src2),
          (ANDrXrs GPR64:$src1, logical_shifted_reg64:$src2)>;
def : Pat<(or GPR64:$src1, logical_shifted_reg64:$src2),
          (ORRrXrs GPR64:$src1, logical_shifted_reg64:$src2)>;
def : Pat<(xor GPR64:$src1, logical_shifted_reg64:$src2),
          (EORrXrs GPR64:$src1, logical_shifted_reg64:$src2)>;

defm ANDSr : LogicalRegGenS<"ands", 0b11, 0b0>;
defm BICSr : LogicalRegGenS<"bics", 0b11, 0b1>;

def ASRV : Shift<0b10, "asr">;
def LSLV : Shift<0b00, "lsl">;
def LSRV : Shift<0b01, "lsr">;
def RORV : Shift<0b11, "ror">;

def : Pat<(sra GPR64:$src1, GPR64:$src2),
          (ASRV GPR64:$src1, GPR64:$src2)>;
def : Pat<(shl GPR64:$src1, GPR64:$src2),
          (LSLV GPR64:$src1, GPR64:$src2)>;
def : Pat<(srl GPR64:$src1, GPR64:$src2),
          (LSRV GPR64:$src1, GPR64:$src2)>;
def : Pat<(rotr GPR64:$src1, GPR64:$src2),
          (RORV GPR64:$src1, GPR64:$src2)>;

/// MOV alias (architectural)
def : InstAlias<"mov $dst, $src", (ORRrXrs GPR64:$dst, XZR, GPR64:$src, 0), 2>;
def : InstAlias<"mvn $Xd, $Xm", (ORNrXrs GPR64:$Xd, XZR, GPR64:$Xm, 0), 3>;

def MOVK : MovImm<0b11, "movk">;
def MOVN : MovImm<0b00, "movn">;
/// TODO need a post encoder when signed symbolic expressions supported.
def MOVZ : MovImm<0b10, "movz">;

def : InstAlias<"movz $Rd, $sym", (MOVZ GPR64:$Rd, movw_symbol_g3:$sym, 48)>;
def : InstAlias<"movz $Rd, $sym", (MOVZ GPR64:$Rd, movw_symbol_g2:$sym, 32)>;
def : InstAlias<"movz $Rd, $sym", (MOVZ GPR64:$Rd, movw_symbol_g1:$sym, 16)>;
def : InstAlias<"movz $Rd, $sym", (MOVZ GPR64:$Rd, movw_symbol_g0:$sym, 0)>;

def : InstAlias<"movn $Rd, $sym", (MOVN GPR64:$Rd, movw_symbol_g3:$sym, 48)>;
def : InstAlias<"movn $Rd, $sym", (MOVN GPR64:$Rd, movw_symbol_g2:$sym, 32)>;
def : InstAlias<"movn $Rd, $sym", (MOVN GPR64:$Rd, movw_symbol_g1:$sym, 16)>;
def : InstAlias<"movn $Rd, $sym", (MOVN GPR64:$Rd, movw_symbol_g0:$sym, 0)>;

def : InstAlias<"movk $Rd, $sym", (MOVK GPR64:$Rd, movw_symbol_g3:$sym, 48)>;
def : InstAlias<"movk $Rd, $sym", (MOVK GPR64:$Rd, movw_symbol_g2:$sym, 32)>;
def : InstAlias<"movk $Rd, $sym", (MOVK GPR64:$Rd, movw_symbol_g1:$sym, 16)>;
def : InstAlias<"movk $Rd, $sym", (MOVK GPR64:$Rd, movw_symbol_g0:$sym, 0)>;

// First group of aliases covers an implicit "lsl #0".
// TODO AArch64 uses a different operand with a predicate, we don't need
// this for the assembler right now.
def : InstAlias<"movk $dst, $imm", (MOVK GPR64:$dst, movimm32_imm:$imm, 0), 0>;
def : InstAlias<"movn $dst, $imm", (MOVN GPR64:$dst, movimm32_imm:$imm, 0)>;
def : InstAlias<"movz $dst, $imm", (MOVZ GPR64:$dst, movimm32_imm:$imm, 0)>;

/// Load Store
def LDRXl : LoadLiteral<0b01, 0, "ldr">;

//---
// (unsigned immediate)
//---
defm LDRX : LoadUI<0b0, 0b01, uimm12s8, "ldr">;
defm STRX : StoreUI<0b0, 0b00, uimm12s8, "str">;

// am_indexed makes sure offset is in range
def : Pat<(load (am_indexed64 GPR64sp:$rn, uimm12s8:$offset)),
          (LDRXui GPR64:$rn, uimm12s8:$offset)>;

def : Pat<(store (i64 GPR64:$rt),
                 (am_indexed64 GPR64sp:$rn, uimm12s8:$offset)),
          (STRXui GPR64:$rt, GPR64sp:$rn, uimm12s8:$offset)>;

/// Multiply only register-register

class MultiplyAccSub <string opstr, bits<1> o0>
    : DataProcessingReg3<0b00, 0b000, o0,
                         (outs GPR64sp:$rd),
                         (ins GPR64sp:$rm, GPR64sp:$ra, GPR64sp:$rn),
                         !strconcat(opstr, " $rd, $rn, $rm, $ra"), []> {
    bits<5> rm;
    bits<5> ra;
    bits<5> rn;
    bits<5> rd;
    let Inst{20-16} = rm;
    let Inst{15} = o0;
    let Inst{14-10} = ra;
    let Inst{9-5} = rn;
    let Inst{4-0} = rd;
}

def MADD : MultiplyAccSub <"madd", 0>;
def MSUB : MultiplyAccSub <"msub", 1>;

def : Pat<(add (mul GPR64sp:$rn, GPR64sp:$rm), GPR64sp:$ra),
          (MADD GPR64sp:$rn, GPR64sp:$rm, GPR64sp:$ra)>;
def : Pat<(sub (mul GPR64sp:$rn, GPR64sp:$rm), GPR64sp:$ra),
          (MSUB GPR64sp:$rn, GPR64sp:$rm, GPR64sp:$ra)>;

// Mul is an alias of MADD it may be possible to refactor to make this
// an alias in tablegen.
class Mulc : DataProcessingReg<0b0, 0b1, 0b1000, 0b011111,
                               (outs GPR64sp:$rd),
                               (ins GPR64sp:$rn, GPR64sp:$rm),
                               "mul $rd, $rn, $rm", []> {
    bits<5> rm;
    bits<5> rn;
    bits<5> rd;
    let Inst{20-16} = rm;
    let Inst{9-5} = rn;
    let Inst{4-0} = rd;
}

def mull : Mulc;

def : Pat<(mul GPR64sp:$rn, GPR64sp:$rm), (mull GPR64sp:$rn, GPR64sp:$rm)>;

// ADR address generation
def ADR : ADRI<0, "adr", adrlabel>;
// ADRP paged address generation
def ADRP : ADRI<1, "adrp", adrplabel>;

def : Pat<(A64adrp tglobaladdr:$label), (ADRP tglobaladdr:$label)>;

//===----------------------------------------------------------------------===//
// Conditional select instructions.
//===----------------------------------------------------------------------===//
def CSEL : CondSelect<0, 0b00, "csel">;

def : Pat<(A64csel GPR64:$src1, GPR64:$src2, (i64 imm:$cond), NZCV),
          (CSEL GPR64:$src1, GPR64:$src2, imm:$cond)>;
//===----------------------------------------------------------------------===//
// Conditional comparison instructions.
//===----------------------------------------------------------------------===//

defm CCMN : CondComparison<0, "ccmn", A64ccmn>;
defm CCMP : CondComparison<1, "ccmp", A64ccmp>;

let isBranch = 1, isTerminator = 1, isBarrier = 1 in {
def B  : BranchImm<"b">;
}

def : Pat<(br bb:$addr), (B bb:$addr)>;

// Branch and Link
let isCall = 1, Defs = [LR], Uses = [SP] in {
  def BL : CallImm<"bl">;
}

def : Pat<(A64call tglobaladdr:$addr),
          (BL tglobaladdr:$addr)>;
def : Pat<(A64call texternalsym:$addr),
          (BL texternalsym:$addr)>;

// Conditional branch
def Bcc : BCond;

def : Pat<(A64brcond bb:$target, imm:$cond, NZCV),
          (Bcc imm:$cond, bb:$target)>;

// Indirect Branches
let isBranch = 1, isTerminator = 1, isBarrier = 1, isIndirectBranch = 1 in {
def BR : BranchReg<0b0000, "br">;
}

def : Pat<(brind GPR64:$Rn), (BR GPR64:$Rn)>;

let isReturn = 1, isTerminator = 1, isBarrier = 1 in {
def RET : BranchReg<0b0010, "ret">;
}

def : Pat<(A64retflag), (RET LR)>;

// RET defaults to x30 (LR)
def : InstAlias<"ret", (RET LR)>;

let isReMaterializable = 1, isCodeGenOnly = 1, isMoveImm = 1,
    isAsCheapAsAMove = 1 in {
def MOVi64imm
    : Pseudo<(outs GPR64:$dst), (ins i64imm:$src),
             [(set GPR64:$dst, imm:$src)]>;
      
} // isReMaterializable, isCodeGenOnly

def MOVaddr
    : Pseudo<(outs GPR64:$dst), (ins i64imm:$hi, i64imm:$low),
             [(set GPR64:$dst, (A64addlow (A64adrp tglobaladdr:$hi),
                                           tglobaladdr:$low))]>;
