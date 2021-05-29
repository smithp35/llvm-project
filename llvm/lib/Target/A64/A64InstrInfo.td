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

// Node definitions
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
def ADJCALLSTACKDOWN : A64PseudoInst<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                                   [(A64callseq_start timm:$amt1, timm:$amt2)]>,
                                   Sched<[]>;
def ADJCALLSTACKUP : A64PseudoInst<(outs), (ins i32imm:$amt1, i32imm:$amt2),
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

// Flag setting
defm ADDSi : AddSubS<"adds", 0>;
defm SUBSi : AddSubS<"subs", 1>;

defm ADD : AddSubRegGen2<"add", 0>;
defm SUB : AddSubRegGen2<"sub", 1>;
defm ADDS : AddSubRegGen2S<"adds", 0>;
defm SUBS : AddSubRegGen2S<"subs", 1>;

/// Arm ARM Logical (immediates)
defm AND : LogicalImm<"and", 0b00, "bic">;
defm ORR : LogicalImm<"orr", 0b01, "orn">;
defm EOR : LogicalImm<"eor", 0b10, "eon">;
defm ANDS : LogicalImmS<"ands", 0b11, "bics">;

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


defm ANDSr : LogicalRegGenS<"ands", 0b11, 0b0>;
defm BICSr : LogicalRegGenS<"bics", 0b11, 0b1>;

/// MOV alias (architectural)
def : InstAlias<"mov $dst, $src", (ORRrXrs GPR64:$dst, XZR, GPR64:$src, 0), 2>;
def : InstAlias<"mvn $Xd, $Xm", (ORNrXrs GPR64:$Xd, XZR, GPR64:$Xm, 0), 3>;

def MOVK : MovImm<0b11, "movk">;
def MOVN : MovImm<0b00, "movn">;
/// TODO need a post encoder when symbolic expressions supported.
def MOVZ : MovImm<0b10, "movz">;

// First group of aliases covers an implicit "lsl #0".
// TODO AArch64 uses a different operand with a predicate, we don't need
// this for the assembler right now.
def : InstAlias<"movk $dst, $imm", (MOVK GPR64:$dst, movimm32_imm:$imm, 0), 0>;
def : InstAlias<"movn $dst, $imm", (MOVN GPR64:$dst, movimm32_imm:$imm, 0)>;
def : InstAlias<"movz $dst, $imm", (MOVZ GPR64:$dst, movimm32_imm:$imm, 0)>;


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
