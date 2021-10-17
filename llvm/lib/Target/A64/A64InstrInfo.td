//=- A64InstrInfo.td - Describe the A64 Instructions     -*- tablegen -----*-=//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// A64 Instruction definitions and custom DAG Nodes.
//
//===----------------------------------------------------------------------===//

// ---
// Target specific node types
// ---

def SDT_A64CCMP                : SDTypeProfile<1, 5,
                                               [SDTCisVT<0, i32>,
                                                SDTCisInt<1>,
                                                SDTCisSameAs<1, 2>,
                                                SDTCisInt<3>,
                                                SDTCisInt<4>,
                                                SDTCisVT<5, i32>]>;

def SDT_A64Brcond              : SDTypeProfile<0, 3,
                                               [SDTCisVT<0, OtherVT>, SDTCisVT<1, i64>,
                                                SDTCisVT<2, i32>]>;

def SDT_A64CSel                : SDTypeProfile<1, 4,
                                              [SDTCisSameAs<0, 1>,
                                               SDTCisSameAs<0, 2>,
                                               SDTCisInt<3>,
                                               SDTCisVT<4, i32>]>;

// SDTBinaryArithWithFlagsOut 2 results - RES1, FLAGS = op LHS, RHS
def SDTBinaryArithWithFlagsOut : SDTypeProfile<2, 2,
                                              [SDTCisSameAs<0, 2>,
                                               SDTCisSameAs<0, 3>,
                                               SDTCisInt<0>, SDTCisVT<1, i32>]>;
// ---
// Target Specific Nodes
// The backend will insert these nodes in various lowering passes
// these nodes will then match the instruction patterns.
// ---

// Return
def A64retflag       : SDNode<"A64ISD::RET_FLAG", SDTNone,
                            [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;
// Call
def A64call          : SDNode<"A64ISD::CALL",
                            SDTypeProfile<0, -1, [SDTCisPtrTy<0>]>,
                            [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue,
                             SDNPVariadic]>;

def A64adrp          : SDNode<"A64ISD::ADRP",   SDTIntUnaryOp, []>;
def A64addlow        : SDNode<"A64ISD::ADDlow", SDTIntBinOp, []>;

def A64ccmp          : SDNode<"A64ISD::CCMP",   SDT_A64CCMP>;
def A64ccmn          : SDNode<"A64ISD::CCMN",   SDT_A64CCMP>;
def A64csel          : SDNode<"A64ISD::CSEL",   SDT_A64CSel>;
def A64brcond        : SDNode<"A64ISD::BRCOND", SDT_A64Brcond,
                            [SDNPHasChain]>;

// Need to return 2 results, arithmetic and flags result.
def A64add_flag      : SDNode<"A64ISD::ADDS",   SDTBinaryArithWithFlagsOut,
                            [SDNPCommutative]>;
def A64sub_flag      : SDNode<"A64ISD::SUBS",   SDTBinaryArithWithFlagsOut>;

// Callsequence start and end.
def A64callseq_start : SDNode<"ISD::CALLSEQ_START",
                            SDCallSeqStart<[ SDTCisVT<0, i32>,
                                             SDTCisVT<1, i32> ]>,
                            [SDNPHasChain, SDNPOutGlue]>;
def A64callseq_end   : SDNode<"ISD::CALLSEQ_END",
                            SDCallSeqEnd<[ SDTCisVT<0, i32>,
                                           SDTCisVT<1, i32> ]>,
                            [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;


include "A64InstrFormats.td"

let Defs = [SP], Uses = [SP], hasSideEffects = 1, isCodeGenOnly = 1 in {
def ADJCALLSTACKDOWN : Pseudo<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                              [(A64callseq_start timm:$amt1, timm:$amt2)]>,
                               Sched<[]>;
def ADJCALLSTACKUP : Pseudo<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                            [(A64callseq_end timm:$amt1, timm:$amt2)]>,
                             Sched<[]>;
}

// ---
// Hint Instruction
// ---

def NOP : Hint<0b0000, 0b000, (outs), (ins), "nop"> {}

// ---
// Arm ARM Add/subtract (immediate)
// ---

// Non flag setting
defm ADDi : AddSub<"add", 0>;
defm SUBi : AddSub<"sub", 1>;

def : Pat<(add GPR64:$src1, addsub_shifted_imm64:$src2),
          (ADDiXri GPR64:$src1, addsub_shifted_imm64:$src2)>;
def : Pat<(sub GPR64:$src1, addsub_shifted_imm64:$src2),
          (SUBiXri GPR64:$src1, addsub_shifted_imm64:$src2)>;

// Flag setting variants
defm ADDSi : AddSubS<"adds", 0>;
defm SUBSi : AddSubS<"subs", 1>;

// Instruction Selection Patterns for Add/Sub Immediate
def : Pat<(A64add_flag GPR64:$src1, addsub_shifted_imm64:$src2),
          (ADDSiXri GPR64:$src1, addsub_shifted_imm64:$src2)>;
def : Pat<(A64sub_flag GPR64:$src1, addsub_shifted_imm64:$src2),
          (SUBSiXri GPR64:$src1, addsub_shifted_imm64:$src2)>;

// Alias of cmp to subs and cmn to adds
def : InstAlias<"cmp $src, $imm", (SUBSiXri XZR, GPR64sp:$src,
                                   addsub_shifted_imm64:$imm)>;
def : InstAlias<"cmn $src, $imm", (ADDSiXri XZR, GPR64sp:$src,
                                   addsub_shifted_imm64:$imm)>;
// ---
// Arm ARM Add/subtract (register)
// ---

defm ADD : AddSubRegGen<"add", 0>;
defm SUB : AddSubRegGen<"sub", 1>;
defm ADDS : AddSubRegGenS<"adds", 0>;
defm SUBS : AddSubRegGenS<"subs", 1>;

// Alias of cmp to subs and cmn to adds
def : InstAlias<"cmp $src, $src2$sh", (SUBSXrs XZR, GPR64:$src,
                GPR64:$src2, arith_shift64:$sh), 5>;
def : InstAlias<"cmn $src, $src2$sh", (ADDSXrs XZR, GPR64:$src,
                GPR64:$src2, arith_shift64:$sh), 5>;

// Instruction Selection Patterns for Add/Sub Register
def : Pat<(add GPR64:$src1, arith_shifted_reg64:$src2),
          (ADDXrs GPR64:$src1, arith_shifted_reg64:$src2)>;
def : Pat<(sub GPR64:$src1, arith_shifted_reg64:$src2),
          (SUBXrs GPR64:$src1, arith_shifted_reg64:$src2)>;

def : Pat<(A64add_flag GPR64:$src1, arith_shifted_reg64:$src2),
          (ADDSXrs GPR64:$src1, arith_shifted_reg64:$src2)>;
def : Pat<(A64sub_flag GPR64:$src1, arith_shifted_reg64:$src2),
          (SUBSXrs GPR64:$src1, arith_shifted_reg64:$src2)>;

// Pseudo for register to register without shift. Expands to
// shift LSL of 0
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

// ---
// Arm ARM Logical (immediates)
// ---

defm AND : LogicalImm<"and", 0b00, "bic">;
defm ORR : LogicalImm<"orr", 0b01, "orn">;
defm EOR : LogicalImm<"eor", 0b10, "eon">;
defm ANDS : LogicalImmS<"ands", 0b11, "bics">;

// Instruction selection patterns for logical (immediate)
def : Pat<(and GPR64:$src1, logical_imm64:$src2),
          (ANDXri GPR64:$src1, logical_imm64:$src2)>;
def : Pat<(or GPR64:$src1,  logical_imm64:$src2),
          (ORRXri GPR64:$src1, logical_imm64:$src2)>;
def : Pat<(xor GPR64:$src1,  logical_imm64:$src2),
          (EORXri GPR64:$src1, logical_imm64:$src2)>;

// Aliases for MOV (architectural)
def : InstAlias<"mov $dst, $imm", (ORRXri GPR64sp:$dst, XZR, logical_imm64:$imm), 0>;

// ---
// Arm ARM Logical (shifted register)
// ---

// Non flag setting
defm ANDr : LogicalRegGen<"and", 0b00, 0b0>;
defm BICr : LogicalRegGen<"bic", 0b00, 0b1>;
defm ORRr : LogicalRegGen<"orr", 0b01, 0b0>;
defm ORNr : LogicalRegGen<"orn", 0b01, 0b1>;
defm EORr : LogicalRegGen<"eor", 0b10, 0b0>;
defm EONr : LogicalRegGen<"eon", 0b10, 0b1>;

// Instruction selection patterns for Arm ARM Logical (shifted register)
def : Pat<(and GPR64:$src1, logical_shifted_reg64:$src2),
          (ANDrXrs GPR64:$src1, logical_shifted_reg64:$src2)>;
def : Pat<(or GPR64:$src1, logical_shifted_reg64:$src2),
          (ORRrXrs GPR64:$src1, logical_shifted_reg64:$src2)>;
def : Pat<(xor GPR64:$src1, logical_shifted_reg64:$src2),
          (EORrXrs GPR64:$src1, logical_shifted_reg64:$src2)>;

// Flag setting variants
defm ANDSr : LogicalRegGenS<"ands", 0b11, 0b0>;
defm BICSr : LogicalRegGenS<"bics", 0b11, 0b1>;

// ---
// Arm Arm shift instructions
// ---

def ASRV : Shift<0b10, "asr">;
def LSLV : Shift<0b00, "lsl">;
def LSRV : Shift<0b01, "lsr">;
def RORV : Shift<0b11, "ror">;

// Instruction selection patterns for shifts
def : Pat<(sra GPR64:$src1, GPR64:$src2),
          (ASRV GPR64:$src1, GPR64:$src2)>;
def : Pat<(shl GPR64:$src1, GPR64:$src2),
          (LSLV GPR64:$src1, GPR64:$src2)>;
def : Pat<(srl GPR64:$src1, GPR64:$src2),
          (LSRV GPR64:$src1, GPR64:$src2)>;
def : Pat<(rotr GPR64:$src1, GPR64:$src2),
          (RORV GPR64:$src1, GPR64:$src2)>;
// ---
// MOV instructions
// ---

/// MOV alias (architectural) as ORR with XZR
def : InstAlias<"mov $dst, $src", (ORRrXrs GPR64:$dst, XZR, GPR64:$src, 0), 2>;
def : InstAlias<"mvn $Xd, $Xm", (ORNrXrs GPR64:$Xd, XZR, GPR64:$Xm, 0), 3>;

def MOVK : MovImm<0b11, "movk">;
def MOVN : MovImm<0b00, "movn">;
// TODO need a post encoder when signed symbolic expressions supported.
// See AArch64 for details.
def MOVZ : MovImm<0b10, "movz">;

// Aliases for symbolic expressions
// "string to match", (instruction to generate)
// Each form will be tried until there is a match, for example
// RD is a GPR64 and sym has one of the operators.
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

// ---
// Load Store Literal
// ---

def LDRXl : LoadLiteral<0b01, 0, "ldr">;

//---
// Load Store (unsigned immediate)
//---

defm LDRX : LoadUI<0b0, 0b01, uimm12s8, "ldr">;
defm STRX : StoreUI<0b0, 0b00, uimm12s8, "str">;

// Instruction selection patterns for load/store unsigned immediate
// am_indexed makes sure offset is in range
def : Pat<(load (am_indexed64 GPR64sp:$rn, uimm12s8:$offset)),
          (LDRXui GPR64:$rn, uimm12s8:$offset)>;

def : Pat<(store (i64 GPR64:$rt),
                 (am_indexed64 GPR64sp:$rn, uimm12s8:$offset)),
          (STRXui GPR64:$rt, GPR64sp:$rn, uimm12s8:$offset)>;

// ---
// Multiply only register-register
// ---

def MADD : MultiplyAccSub <"madd", 0>;
def MSUB : MultiplyAccSub <"msub", 1>;

// Instruction selections for MADD, MSUB
def : Pat<(add (mul GPR64sp:$rn, GPR64sp:$rm), GPR64sp:$ra),
          (MADD GPR64sp:$rn, GPR64sp:$rm, GPR64sp:$ra)>;
def : Pat<(sub (mul GPR64sp:$rn, GPR64sp:$rm), GPR64sp:$ra),
          (MSUB GPR64sp:$rn, GPR64sp:$rm, GPR64sp:$ra)>;

// actually an alias of multiply accumulate but hard coded here
def mull : Mulc;

// Instruction selection patterns for mull
def : Pat<(mul GPR64sp:$rn, GPR64sp:$rm), (mull GPR64sp:$rn, GPR64sp:$rm)>;

// ---
// ADR address generation
// ---
def ADR : ADRI<0, "adr", adrlabel>;

// ---
// ADRP paged address generation
// ---

def ADRP : ADRI<1, "adrp", adrplabel>;

// Instruction selection pattern for ADRP
def : Pat<(A64adrp tglobaladdr:$label), (ADRP tglobaladdr:$label)>;

//---
// Conditional select instructions.
//---

def CSEL : CondSelect<0, 0b00, "csel">;

// Instruction selection pattern for conditional select
def : Pat<(A64csel GPR64:$src1, GPR64:$src2, (i64 imm:$cond), NZCV),
          (CSEL GPR64:$src1, GPR64:$src2, imm:$cond)>;

//---
// Conditional comparison instructions.
//---

defm CCMN : CondComparison<0, "ccmn", A64ccmn>;
defm CCMP : CondComparison<1, "ccmp", A64ccmp>;

// ---
// Branch instructions
// ---

// branch immediate
let isBranch = 1, isTerminator = 1, isBarrier = 1 in {
def B  : BranchImm<"b">;
}

// Instruction selection pattern for branch
def : Pat<(br bb:$addr), (B bb:$addr)>;

// Branch and Link, also a call
let isCall = 1, Defs = [LR], Uses = [SP] in {
  def BL : CallImm<"bl">;
}

// Instuction selection patterns for Call
def : Pat<(A64call tglobaladdr:$addr),
          (BL tglobaladdr:$addr)>;
def : Pat<(A64call texternalsym:$addr),
          (BL texternalsym:$addr)>;

// Conditional branch
def Bcc : BCond;

def : Pat<(A64brcond bb:$target, imm:$cond, NZCV),
          (Bcc imm:$cond, bb:$target)>;

// ---
// Indirect Branches, including return
// ---
let isBranch = 1, isTerminator = 1, isBarrier = 1, isIndirectBranch = 1 in {
def BR : BranchReg<0b0000, "br">;
}

def : Pat<(brind GPR64:$Rn), (BR GPR64:$Rn)>;

let isReturn = 1, isTerminator = 1, isBarrier = 1 in {
def RET : BranchReg<0b0010, "ret">;
}

def : Pat<(A64retflag), (RET LR)>;

// ---
// RET as alias of branch indirect with default of x30 (LR)
// ---
def : InstAlias<"ret", (RET LR)>;

// ---
// Pseudo instruction to create a 64-bit constant as a series of moves
// ---
let isReMaterializable = 1, isCodeGenOnly = 1, isMoveImm = 1,
    isAsCheapAsAMove = 1 in {
def MOVi64imm
    : Pseudo<(outs GPR64:$dst), (ins i64imm:$src),
             [(set GPR64:$dst, imm:$src)]>;

} // isReMaterializable, isCodeGenOnly

// ---
// Pseudo instruction to match custom adrp, add selection DAG nodes
// ---
def MOVaddr
    : Pseudo<(outs GPR64:$dst), (ins i64imm:$hi, i64imm:$low),
             [(set GPR64:$dst, (A64addlow (A64adrp tglobaladdr:$hi),
                                           tglobaladdr:$low))]>;
