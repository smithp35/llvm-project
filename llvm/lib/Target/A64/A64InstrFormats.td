//===- A64InstrFormats.td - A64 Instruction Formats ----------*- tblgen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Describe A64 instructions format here
//

class A64Inst<dag outs, dag ins, string asmstr, list<dag> pattern>
    : Instruction {
    field bits<32> Inst;

    // Mask of bits that cause an encoding to be UNPREDICTABLE.
    // If a bit is set, then if the corresponding bit in the
    // target encoding differs from its value in the "Inst" field,
    // the instruction is UNPREDICTABLE (SoftFail in abstract parlance).
    field bits<32> Unpredictable = 0;
    // SoftFail is the generic name for this field, but we alias it so
    // as to make it more obvious what it means in ARM-land.
    field bits<32> SoftFail = Unpredictable;


    let Namespace = "A64";
    dag OutOperandList = outs;
    dag InOperandList  = ins;
    let AsmString      = asmstr;
    let Pattern        = pattern;
    let Size           = 4;
}

// A64 pseudo instructions format
class A64PseudoInst<dag outs, dag ins, list<dag> pattern, string asmstr = "">
    : A64Inst<outs, ins, asmstr, pattern> {
    let isPseudo = 1;
}

class DataProcessingReg <bits<1> op0, bits<1> op1, bits<4> op2, bits<6> op3,
                         dag outs, dag ins, string asmstr, list<dag> pattern>
    : A64Inst<outs, ins, asmstr, pattern> {
    let Inst{31} = 0b1;
    let Inst{30} = op0;
    let Inst{28} = op1;
    let Inst{27-25} = 0b101;
    let Inst{24-21} = op2;
    let Inst{15-10} = op3;
}

class DataProcessingImm <bits<3> opcode, dag outs, dag ins, string asmstr,
                         list<dag> pattern>
    : A64Inst<outs, ins, asmstr, pattern> {
    let hasSideEffects = 0;
    let mayLoad = 0;
    let mayStore = 0;
    let Inst{31} = 0b1;
    let Inst{28-26} = 0b100;
    let Inst{25-23} = opcode;
}

class LoadStore <dag outs, dag ins, string asmstr, list<dag> pattern>
    : A64Inst<outs, ins, asmstr, pattern> {
    let Inst{27} = 0b1;
    let Inst{25} = 0b0;
}

class Branch <dag outs, dag ins, string asmstr, list<dag> pattern>
    : A64Inst<outs, ins, asmstr, pattern> {
    let Inst{27-25} = 0b101;
}

class Hint <bits<4> CRm, bits<3> op2, dag outs, dag ins, string asmstr,
            list<dag> pattern>
    : A64Inst<outs, ins, asmstr, pattern> {
    let Inst{31-29} = 0b110;
    let Inst{28-26} = 0b101;
    let Inst{25-12} = 0b01000000110010;
    let Inst{11-8}  = CRm;
    let Inst{7-5}   = op2;
    let Inst{4-0}   = 0b11111;
}



