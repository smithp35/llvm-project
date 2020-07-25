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


class AddSubImmGen<string opstr, bits<1> op, bits<1> S>
    : DataProcessingImm<0b010,
                        (outs GPR64sp:$rd),
                        (ins GPR64sp:$rn, uimm12:$imm),
                        !strconcat(opstr, " $rd, $rn, $imm"), []> {
    bits<5>  rn;
    bits<5>  rd;
    bits<12> imm;
    let Inst{30} = op;
    let Inst{29} = S;
    let Inst{22} = 0b0; // hard code shift to 0
    let Inst{21-10} = imm;
    let Inst{9-5} = rn;
    let Inst{4-0} = rd;
}

// Flag setting
class AddSubImmS<string opstr, bits<1> op>
    : AddSubImmGen<opstr, op, 0b1> {
    let isCompare = 1;
    let Defs = [NZCV];
}

// Non flag setting
class AddSubImm<string opstr, bits<1> op>
    : AddSubImmGen<opstr, op, 0b0> {
}

// Non flag setting
def ADDI : AddSubImm<"add", 0>;
def SUBI : AddSubImm<"sub", 1>;

// Flag setting
def ADDIS : AddSubImmS<"adds", 0>;
def SUBIS : AddSubImmS<"subs", 1>;

// Simplified, we don't support shifted or extended registers
class AddSubRegGen <string opstr, bits<1> op, bits<1> S>
    : DataProcessingReg<op, 0b0, 0b1000, 0b000000,
                        (outs GPR64sp:$rd),
                        (ins GPR64sp:$rn, GPR64sp:$rm),
                        !strconcat(opstr, " $rd, $rn, $rm"), []> {
    bits<5>  rn;
    bits<5>  rd;
    bits<5>  rm;
    let Inst{30} = op;
    let Inst{29} = S;
    let Inst{23-22} = 0b00; // hard code shift to 0
    let Inst{20-16} = rm;
    let Inst{9-5} = rn;
    let Inst{4-0} = rd;
}

// Flag setting
class AddSubRegS<string opstr, bits<1> op>
    : AddSubRegGen<opstr, op, 0b1> {
    let isCompare = 1;
    let Defs = [NZCV];
}

// Non flag setting
class AddSubReg<string opstr, bits<1> op>
    : AddSubRegGen<opstr, op, 0b0> {
}

def ADDR : AddSubReg<"add", 0>;
def SUBR : AddSubReg<"sub", 1>;

// Flag setting
def ADDRS : AddSubRegS<"adds", 0>;
def SUBRS : AddSubRegS<"subs", 1>;
