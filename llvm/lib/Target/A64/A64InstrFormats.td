//===- A64InstrFormats.td - A64 Instruction Formats ----------*- tblgen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

/// Helper class to describe immediates within a range
class AsmImmRange<int Low, int High> : AsmOperandClass {
  let Name = "Imm" # Low # "_" # High;
  let DiagnosticType = "InvalidImm" # Low # "_" # High;
  let RenderMethod = "addImmOperands";
  let PredicateMethod = "isImmInRange<" # Low # "," # High # ">";
}

class BranchTarget<int N> : AsmOperandClass {
  let Name = "BranchTarget" # N;
  let DiagnosticType = "InvalidLabel";
  let PredicateMethod = "isBranchTarget<" # N # ">";
}

class PCRelLabel<int N> : BranchTarget<N> {
  let Name = "PCRelLabel" # N;
}

// TODO conditional branches
def BranchTarget26Operand : BranchTarget<26>;

def PCRelLabel19Operand   : PCRelLabel<19>;

// Load/Store unsigned scaled offset operand

def am_indexed64 : ComplexPattern<i64, 2, "SelectAddrModeIndexed64", []>;

// Only 64-bit 8-bytes supported at the moment
class UImm12OffsetOperand<int Scale> : AsmOperandClass {
  let Name = "UImm12Offset" # Scale;
  let RenderMethod = "addUImm12OffsetOperands<" # Scale # ">";
  let PredicateMethod = "isUImm12Offset<" # Scale # ">";
  let DiagnosticType = "InvalidMemoryIndexed" # Scale;
}

def UImm12OffsetScale8Operand : UImm12OffsetOperand<8>;

class uimm12_scaled<int Scale> : Operand<i64> {
  let ParserMatchClass
   = !cast<AsmOperandClass>("UImm12OffsetScale" # Scale # "Operand");
  let EncoderMethod
   = "getLdStUImm12OpValue";
  let PrintMethod = "printUImm12Offset<" # Scale # ">";
}

def uimm12s8 : uimm12_scaled<8>;

// Load literal address: 19-bit immediate. The low two bits of the target
// offset are implied zero and so are not part of the immediate.
def am_ldrlit : Operand<iPTR> {
  let EncoderMethod = "getLoadLiteralOpValue";
  let DecoderMethod = "DecodePCRelLabel19";
  let PrintMethod = "printAlignedLabel";
  let ParserMatchClass = PCRelLabel19Operand;
  let OperandType = "OPERAND_PCREL";
}

// ADD/Sub immediate shifter operand:
//  second operand:
//  {7-6} - shift type: 00 = lsl
//  {5-0} - imm6: #0 or #12


// Shifter operand for arithmetic shifted encodings.
def ShifterOperand : AsmOperandClass {
  let Name = "Shifter";
}

let DiagnosticType = "AddSubSecondSource" in {
  def AddSubImmOperand : AsmOperandClass {
    let Name = "AddSubImm";
    let ParserMethod = "tryParseImmWithOptionalShift";
    let RenderMethod = "addImmWithOptionalShiftOperands<12>";
  }
  def AddSubImmNegOperand : AsmOperandClass {
    let Name = "AddSubImmNeg";
    let ParserMethod = "tryParseImmWithOptionalShift";
    let RenderMethod = "addImmNegWithOptionalShiftOperands<12>";
  }
}

class addsub_shifted_imm<ValueType Ty>
    : Operand<Ty>, ComplexPattern<Ty, 2, "SelectArithImmed", [imm]>
  {
  let PrintMethod = "printAddSubImm";
  let EncoderMethod = "getAddSubImmOpValue";
  let ParserMatchClass = AddSubImmOperand;
  let MIOperandInfo = (ops i32imm, i32imm);
}

class addsub_shifted_imm_neg<ValueType Ty>
    : Operand<Ty> {
  let EncoderMethod = "getAddSubImmOpValue";
  let ParserMatchClass = AddSubImmNegOperand;
  let MIOperandInfo = (ops i32imm, i32imm);
}

def addsub_shifted_imm64 : addsub_shifted_imm<i64>;
def addsub_shifted_imm_neg64 : addsub_shifted_imm_neg<i64>;

// Shifter operand for arithmetic register shifted encodings.
class ArithmeticShifterOperand : AsmOperandClass {
  let SuperClasses = [ShifterOperand];
  let Name = "ArithmeticShifter";
  let PredicateMethod = "isArithmeticShifter";
  let RenderMethod = "addShifterOperands";
 let DiagnosticType = "AddSubRegShift";
}

def ArithmeticShifterOperand64 : ArithmeticShifterOperand;

// Shifter operand for logical register shifted encodings.
class LogicalShifterOperand : AsmOperandClass {
  let SuperClasses = [ShifterOperand];
  let Name = "LogicalShifter";
  let PredicateMethod = "isLogicalShifter";
  let RenderMethod = "addShifterOperands";
  let DiagnosticType = "AddSubRegShift";
}

def LogicalShifterOperand64 : LogicalShifterOperand;

// An arithmetic shifter operand:
//  {7-6} - shift type: 00 = lsl, 01 = lsr, 10 = asr
//  {5-0} - imm6
class arith_shift : Operand<i64> {
  let PrintMethod = "printShifter";
  let ParserMatchClass = !cast<AsmOperandClass>(
                         "ArithmeticShifterOperand64");
}

def arith_shift64 : arith_shift;

class arith_shifted_reg<ValueType Ty, RegisterClass regclass>
    : Operand<Ty>, ComplexPattern<Ty, 2, "SelectArithShiftedRegister", []>
      {
  let PrintMethod = "printShiftedRegister";
  let MIOperandInfo = (ops regclass, !cast<Operand>("arith_shift64"));
}

def arith_shifted_reg64 : arith_shifted_reg<i64, GPR64>;

// A logical shifter operand:
//  {7-6} - shift type: 00 = lsl, 01 = lsr, 10 = asr, 11 = ror
//  {5-0} - imm6
class logical_shift : Operand<i32> {
  let PrintMethod = "printShifter";
  let ParserMatchClass = !cast<AsmOperandClass>(
                         "LogicalShifterOperand64");
}

def logical_shift64 : logical_shift;

class logical_shifted_reg<ValueType Ty, RegisterClass regclass, Operand shiftop>
    : Operand<Ty>, ComplexPattern<Ty, 2, "SelectLogicalShiftedRegister", []> {
  let PrintMethod = "printShiftedRegister";
  let MIOperandInfo = (ops regclass, shiftop);
}

def logical_shifted_reg64 : logical_shifted_reg<i64, GPR64, logical_shift64>;

// ADR operand
def AdrOperand : AsmOperandClass {
  let Name = "AdrLabel";
  let ParserMethod = "tryParseAdrLabel";
  let DiagnosticType = "InvalidLabel";
}
def adrlabel : Operand<i64> {
  let EncoderMethod = "getAdrLabelOpValue";
  let ParserMatchClass = AdrOperand;
}

// ADRP operand
def AdrpOperand : AsmOperandClass {
  let Name = "AdrpLabel";
  let ParserMethod = "tryParseAdrpLabel";
  let DiagnosticType = "InvalidLabel";
}
def adrplabel : Operand<i64> {
  let EncoderMethod = "getAdrLabelOpValue";
  let PrintMethod = "printAdrpLabel";
  let ParserMatchClass = AdrpOperand;
  let OperandType = "OPERAND_PCREL";
}

//===----------------------------------------------------------------------===//
//  Describe A64 instructions format here
//

class InstSubst<string Asm, dag Result, bit EmitPriority = 0>
  : InstAlias<Asm, Result, EmitPriority>;

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

class BaseAddSubImm<bit isSub, bit S, string opstr,
                    addsub_shifted_imm immtype>
    : A64Inst<(outs GPR64sp:$rd),
              (ins GPR64sp:$rn, immtype:$imm),
              !strconcat(opstr, " $rd, $rn, $imm"), []> {
  bits<5>  rd;
  bits<5>  rn;
  bits<14> imm;
  let Inst{31} = 0b1; // hard code sf = 1
  let Inst{30} = isSub;
  let Inst{29} = S;
  let Inst{28-24} = 0b10001;
  let Inst{23-22} = imm{13-12};
  let Inst{21-10} = imm{11-0};
  let Inst{9-5} = rn;
  let Inst{4-0} = rd;
  let DecoderMethod = "DecodeAddSubImmShift";
}

multiclass AddSub<string opstr, bit isSub> {
  let AddedComplexity = 6 in
  def Xri : BaseAddSubImm<isSub, 0, opstr, addsub_shifted_imm64>;
}

multiclass AddSubS<string opstr, bit isSub> {
  def Xri : BaseAddSubImm<isSub, 1, opstr, addsub_shifted_imm64> {
    let Defs = [NZCV];
  }
}

let DiagnosticType = "LogicalSecondSource" in {
  def LogicalImm64Operand : AsmOperandClass {
    let Name = "LogicalImm64";
    let PredicateMethod = "isLogicalImm<int64_t>";
    let RenderMethod = "addLogicalImmOperands<int64_t>";
  }
  def LogicalImm64NotOperand : AsmOperandClass {
    let Name = "LogicalImm64Not";
    let PredicateMethod = "isLogicalImm<int64_t>";
    let RenderMethod = "addLogicalImmNotOperands<int64_t>";
  }
}

def logical_imm64 : Operand<i64> {
  let PrintMethod = "printLogicalImm";
  let ParserMatchClass = LogicalImm64Operand;
}

def logical_imm64_not : Operand<i64> {
  let ParserMatchClass = LogicalImm64NotOperand;
}

class LogicalImmGenBase <string opstr, bits<2> opc>
    : A64Inst<(outs GPR64sp:$rd),
              (ins  GPR64:$rn, logical_imm64:$imm),
              !strconcat(opstr, " $rd, $rn, $imm"), []> {
  bits<5> Rd;
  bits<5> Rn;
  bits<13> imm;
  let Inst{31}  = 0b1; // hard code sf = 1
  let Inst{30-29} = opc;
  let Inst{28-23} = 0b100100;
  let Inst{22} = imm{12};
  let Inst{21-16} = imm{11-6};
  let Inst{15-10} = imm{5-0};
  let Inst{9-5} = Rn;
  let Inst{4-0} = Rd;

  let DecoderMethod = "DecodeLogicalImmInstruction";
}

multiclass LogicalImm<string opstr, bits<2> opc, string alias> {
   def Xri : LogicalImmGenBase<opstr, opc>;

   def : InstSubst<alias # " $rd, $rn, $imm",
                  (!cast<Instruction>(NAME # "Xri") GPR64sp:$rd, GPR64:$rn,
                       logical_imm64_not:$imm), 0>;
}

multiclass LogicalImmS<string opstr, bits<2> opc, string alias> {
   def Xri : LogicalImmGenBase<opstr, opc> {
     let Defs = [NZCV];
   }
   def : InstSubst<alias # " $rd, $rn, $imm",
                  (!cast<Instruction>(NAME # "Xri") GPR64sp:$rd, GPR64:$rn,
                       logical_imm64_not:$imm), 0>;
}

class LogicalRegGenBase <string opstr, bits<2> opc, bits<1> N>
    : A64Inst<(outs GPR64:$rd),
              (ins GPR64:$rn, logical_shifted_reg64:$rm),
              !strconcat(opstr, " $rd, $rn, $rm"), []> {

    // Operands in order to match the ins, outs. Names must be different
    // to use in-order mapping.
    bits<5>  dst;
    bits<5>  src1;
    bits<5>  src2;
    bits<8>  shift;
    let Inst{31} = 0b1; // hard code sf = 1
    let Inst{30-29} = opc;
    let Inst{28-24} = 0b01010;
    let Inst{23-22} = shift{7-6};
    let Inst{21} = N;
    let Inst{20-16} = src2;
    let Inst{15-10} = shift{5-0};
    let Inst{9-5} = src1;
    let Inst{4-0} = dst;
}

class LogicalRegAlias<string asm, Instruction inst, RegisterClass regtype>
    : InstAlias<asm#"\t$dst, $src1, $src2",
                (inst regtype:$dst, regtype:$src1, regtype:$src2, 0)>;

multiclass LogicalRegGen<string opstr, bits<2> opc, bits<1> N> {
  def Xrs : LogicalRegGenBase<opstr, opc, N>;
  def : LogicalRegAlias<opstr,
        !cast<Instruction>(NAME#"Xrs"), GPR64>;
}

// Flag Setting Variant of logical register
multiclass LogicalRegGenS<string opstr, bits<2> opc, bits<1> N> {
  def Xrs : LogicalRegGenBase<opstr, opc, N> {
    let Defs = [NZCV];
  }
  def : LogicalRegAlias<opstr,
        !cast<Instruction>(NAME#"Xrs"), GPR64>;
}

/// Arm ARM Data Add/subtract (shifted register)
class AddSubRegBase<string opstr, bits<1> op, bits<1> S>
      : A64Inst<(outs GPR64:$rd),
                (ins GPR64:$rn, arith_shifted_reg64:$rm),
                !strconcat(opstr, " $rd, $rn, $rm"), []> {
    // Operands in order to match the ins, outs. Names must be different
    // to use in-order mapping.
    bits<5>  dst;
    bits<5>  src1;
    bits<5>  src2;
    bits<8>  shift;
    let Inst{31} = 0b1; // hard code sf = 1
    let Inst{30} = op;
    let Inst{29} = S;
    let Inst{28-24} = 0b01011;
    let Inst{23-22} = shift{7-6};
    let Inst{21} = 0;
    let Inst{20-16} = src2;
    let Inst{15-10} = shift{5-0};
    let Inst{9-5} = src1;
    let Inst{4-0} = dst;
}

class AddSubRegAlias<string asm, Instruction inst, RegisterClass regtype>
    : InstAlias<asm#"\t$dst, $src1, $src2",
                (inst regtype:$dst, regtype:$src1, regtype:$src2, 0)>;

multiclass AddSubRegGen2<string opstr, bits<1> op> {
  def Xrs : AddSubRegBase<opstr, op, 0b0>;
  def : AddSubRegAlias<opstr, !cast<Instruction>(NAME#"Xrs"), GPR64>;
}

// Flag Setting Variant of Add Sub register
multiclass AddSubRegGen2S<string opstr, bits<1> op> {
  def Xrs : AddSubRegBase<opstr, op, 0b1> {
    let Defs = [NZCV];
  }
  def : AddSubRegAlias<opstr, !cast<Instruction>(NAME#"Xrs"), GPR64>;
}

// A64 pseudo instructions format
class A64PseudoInst<dag outs, dag ins, list<dag> pattern, string asmstr = "">
    : A64Inst<outs, ins, asmstr, pattern> {
    let isPseudo = 1;
}

// Move immediate
def MovWSymbolG3AsmOperand : AsmOperandClass {
  let Name = "MovWSymbolG3";
  let RenderMethod = "addImmOperands";
}

def movw_symbol_g3 : Operand<i32> {
  let ParserMatchClass = MovWSymbolG3AsmOperand;
}

def MovWSymbolG2AsmOperand : AsmOperandClass {
  let Name = "MovWSymbolG2";
  let RenderMethod = "addImmOperands";
}

def movw_symbol_g2 : Operand<i32> {
  let ParserMatchClass = MovWSymbolG2AsmOperand;
}

def MovWSymbolG1AsmOperand : AsmOperandClass {
  let Name = "MovWSymbolG1";
  let RenderMethod = "addImmOperands";
}

def movw_symbol_g1 : Operand<i32> {
  let ParserMatchClass = MovWSymbolG1AsmOperand;
}

def MovWSymbolG0AsmOperand : AsmOperandClass {
  let Name = "MovWSymbolG0";
  let RenderMethod = "addImmOperands";
}

def movw_symbol_g0 : Operand<i32> {
  let ParserMatchClass = MovWSymbolG0AsmOperand;
}

def MovImm64ShifterOperand : AsmOperandClass {
  let SuperClasses = [ShifterOperand];
  let Name = "MovImm64Shifter";
  let RenderMethod = "addShifterOperands";
  let DiagnosticType = "InvalidMovImm64Shift";
}

def movimm32_imm : Operand<i32> {
  let ParserMatchClass = AsmImmRange<0, 65535>;
  let EncoderMethod = "getMoveWideImmOpValue";
  let PrintMethod = "printImm";
}
def movimm64_shift : Operand<i32> {
  let PrintMethod = "printShifter";
  let ParserMatchClass = MovImm64ShifterOperand;
}

class MovImm<bits<2> opc, string opstr> :
  A64Inst<(outs GPR64:$rd), (ins movimm32_imm:$imm, movimm64_shift:$shift),
          !strconcat(opstr, " $rd, $imm$shift"), []> {
  bits<5> rd;
  bits<16> imm;
  bits<6> shift;
  let Inst{31} = 0b1;
  let Inst{30-29} = opc;
  let Inst{28-23} = 0b100101;
  let Inst{22-21} = shift{5-4};
  let Inst{20-5} = imm;
  let Inst{4-0} = rd;

  let DecoderMethod = "DecodeMoveImmInstruction";
}

let mayLoad = 1, mayStore = 0, hasSideEffects = 0, AddedComplexity = 20 in
class LoadLiteral<bits<2> opc, bit V, string opstr>
    : A64Inst<(outs GPR64:$rt), (ins am_ldrlit:$label),
        !strconcat(opstr, "\t$rt, $label"), []> {
  bits<5> rt;
  bits<19> label;
  let Inst{31-30} = opc;
  let Inst{29-27} = 0b011;
  let Inst{26}    = V;
  let Inst{25-24} = 0b00;
  let Inst{23-5}  = label;
  let Inst{4-0}   = rt;
}

class BaseLoadStoreUI<bit V, bits<2> opc, dag oops, dag iops,
                      string opstr>
    : A64Inst<oops, iops, !strconcat(opstr, "\t$rt, [$rn, $imm]"), []> {
  bits<5> rt;
  bits<5> rn;
  bits<12> imm;

  let Inst{31-30} = 0b11;
  let Inst{29-27} = 0b111;
  let Inst{26}    = V;
  let Inst{25-24} = 0b01;
  let Inst{23-22} = opc;
  let Inst{21-10} = imm;
  let Inst{9-5}   = rn;
  let Inst{4-0}   = rt;

  let DecoderMethod = "DecodeUnsignedLdStInstruction";
}

multiclass LoadUI<bit V, bits<2> opc,
                  Operand indextype, string opstr> {
  let AddedComplexity = 10, mayLoad = 1, mayStore = 0, hasSideEffects = 0 in
  def ui : BaseLoadStoreUI<V, opc, (outs GPR64:$rt),
                           (ins GPR64sp:$rn, indextype:$imm),
                           opstr>;

  def : InstAlias<opstr # "\t$rt, [$rn]",
                  (!cast<Instruction>(NAME # "ui") GPR64:$rt, GPR64sp:$rn, 0)>;
}

multiclass StoreUI<bit V, bits<2> opc,
                  Operand indextype, string opstr> {
  let AddedComplexity = 10, mayLoad = 0, mayStore = 1, hasSideEffects = 0 in
  def ui : BaseLoadStoreUI<V, opc, (outs),
                           (ins GPR64:$rt, GPR64sp:$rn, indextype:$imm),
                           opstr>;

  def : InstAlias<opstr # "\t$rt, [$rn]",
                  (!cast<Instruction>(NAME # "ui") GPR64:$rt, GPR64sp:$rn, 0)>;
}

//---
// Address generation.
//---

class ADRI<bit op, string opstr, Operand labelop>
    : A64Inst<(outs GPR64:$xd), (ins labelop:$label),
              !strconcat(opstr, "\t$xd, $label"),
              []> {
  bits<5>  xd;
  bits<21> label;
  let Inst{31}    = op;
  let Inst{30-29} = label{1-0};
  let Inst{28-24} = 0b10000;
  let Inst{23-5}  = label{20-2};
  let Inst{4-0}   = xd;

  let DecoderMethod = "DecodeAdrInstruction";
}

def CondCode : AsmOperandClass {
  let Name = "CondCode";
  let DiagnosticType = "InvalidCondCode";
}

// Condition code.
// 4-bit immediate. Pretty-printed as <cc>
def ccode : Operand<i32> {
  let PrintMethod = "printCondCode";
  let ParserMatchClass = CondCode;
}

// Conditional branch target. 19-bit immediate. The low two bits of the target
// offset are implied zero and so are not part of the immediate.
def am_brcond : Operand<OtherVT> {
  let EncoderMethod = "getCondBranchTargetOpValue";
  let DecoderMethod = "DecodePCRelLabel19";
  let PrintMethod = "printAlignedLabel";
  let ParserMatchClass = PCRelLabel19Operand;
  let OperandType = "OPERAND_PCREL";
}

class BCond : A64Inst<(outs), (ins ccode:$cond, am_brcond:$target),
                     "b.$cond\t$target",[]> {
  let isBranch = 1;
  let isTerminator = 1;
  let Uses = [NZCV];

  bits<4> cond;
  bits<19> target;
  let Inst{31-24} = 0b01010100;
  let Inst{23-5} = target;
  let Inst{4} = 0;
  let Inst{3-0} = cond;
}

//---
// Unconditional branch (immediate) instructions.
//---
def am_b_target : Operand<OtherVT> {
  let EncoderMethod = "getBranchTargetOpValue";
  let PrintMethod = "printAlignedLabel";
  let ParserMatchClass = BranchTarget26Operand;
  let OperandType = "OPERAND_PCREL";
}

def am_bl_target : Operand<i64> {
  let EncoderMethod = "getBranchTargetOpValue";
  let PrintMethod = "printAlignedLabel";
  let ParserMatchClass = BranchTarget26Operand;
  let OperandType = "OPERAND_PCREL";
}

class BImm<bit op, dag ins, string opstr>
    : A64Inst<(outs), ins, !strconcat(opstr, "\t$addr"),[]> {
  bits<26> addr;
  let Inst{31}    = op;
  let Inst{30-26} = 0b00101;
  let Inst{25-0}  = addr;

  let DecoderMethod = "DecodeUnconditionalBranch";
}

class CallImm<string opstr>
    : BImm<0b1, (ins am_bl_target:$addr), opstr>;

class BranchImm<string opstr>
    : BImm<0b0, (ins am_b_target:$addr), opstr>;

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

class DataProcessingReg3 <bits<2> op54, bits<3> op31, bits<1> o0,
                          dag outs, dag ins, string asmstr,
                          list<dag> pattern>
    : A64Inst<outs, ins, asmstr, pattern> {
    let hasSideEffects = 0;
    let mayLoad = 0;
    let mayStore = 0;
    let Inst{31} = 0b1;
    let Inst{30-29} = op54;
    let Inst{28-24} = 0b11011;
    let Inst{23-21} = op31;
    let Inst{15} = o0;
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
