//==-- A64InstPrinter.cpp - Convert A64 MCInst to assembly syntax ----------==//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This class prints an A64 MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#include "A64InstPrinter.h"
#include "Utils/A64BaseInfo.h"
#include "MCTargetDesc/A64AddressingModes.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrDesc.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/Format.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "asm-printer"

#define GET_INSTRUCTION_NAME
#define PRINT_ALIAS_INSTR
#include "A64GenAsmWriter.inc"

A64InstPrinter::A64InstPrinter(const MCAsmInfo &MAI, const MCInstrInfo &MII,
                               const MCRegisterInfo &MRI)
    : MCInstPrinter(MAI, MII, MRI) {}

void A64InstPrinter::printRegName(raw_ostream &OS, unsigned RegNo) const {
  // This is for .cfi directives.
  OS << getRegisterName(RegNo);
}

void A64InstPrinter::printInst(const MCInst *MI, uint64_t Address,
                               StringRef Annot, const MCSubtargetInfo &STI,
                               raw_ostream &O) {
  // Check for special encodings and print the canonical alias instead.
  unsigned Opcode = MI->getOpcode();

  // Symbolic operands for MOVZ, MOVN and MOVK already imply a shift
  // (e.g. :gottprel_g1: is always going to be "lsl #16") so it should not be
  // printed.
  if ((Opcode == A64::MOVZ || Opcode == A64::MOVN) &&
      MI->getOperand(1).isExpr()) {
    if (Opcode == A64::MOVZ)
      O << "\tmovz\t";
    else
      O << "\tmovn\t";

    O << getRegisterName(MI->getOperand(0).getReg()) << ", #";
    MI->getOperand(1).getExpr()->print(O, &MAI);
    return;
  }

  if ((Opcode == A64::MOVK) && MI->getOperand(2).isExpr()) {
    O << "\tmovk\t" << getRegisterName(MI->getOperand(0).getReg()) << ", #";
    MI->getOperand(2).getExpr()->print(O, &MAI);
    return;
  }

  if (!printAliasInstr(MI, Address, O))
    printInstruction(MI, Address, O);

  printAnnotation(O, Annot);
}

void A64InstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
                                  raw_ostream &O) {
  const MCOperand &Op = MI->getOperand(OpNo);
  if (Op.isReg()) {
    unsigned Reg = Op.getReg();
    O << getRegisterName(Reg);
  } else if (Op.isImm()) {
    printImm(MI, OpNo, O);
  } else {
    assert(Op.isExpr() && "unknown operand kind in printOperand");
    Op.getExpr()->print(O, &MAI);
  }
}

void A64InstPrinter::printCondCode(const MCInst *MI, unsigned OpNum,
                                   raw_ostream &O) {
  A64CC::CondCode CC = (A64CC::CondCode)MI->getOperand(OpNum).getImm();
  O << A64CC::getCondCodeName(CC);
}

void A64InstPrinter::printImm(const MCInst *MI, unsigned OpNo, raw_ostream &O) {
  const MCOperand &Op = MI->getOperand(OpNo);
  O << "#" << formatImm(Op.getImm());
}

void A64InstPrinter::printShifter(const MCInst *MI, unsigned OpNum,
                                  raw_ostream &O) {
  unsigned Val = MI->getOperand(OpNum).getImm();
  // LSL #0 should not be printed.
  if (A64_AM::getShiftType(Val) == A64_AM::LSL &&
      A64_AM::getShiftValue(Val) == 0)
    return;
  O << ", " << A64_AM::getShiftExtendName(A64_AM::getShiftType(Val)) << " #"
    << A64_AM::getShiftValue(Val);
}

void A64InstPrinter::printLogicalImm(const MCInst *MI, unsigned OpNum,
                                     raw_ostream &O) {
  uint64_t Val = MI->getOperand(OpNum).getImm();
  O << "#0x";
  O.write_hex(A64_AM::decodeLogicalImmediate(Val, 8 * sizeof(int64_t)));
}

void A64InstPrinter::printShiftedRegister(const MCInst *MI, unsigned OpNum,
                                          raw_ostream &O) {
  O << getRegisterName(MI->getOperand(OpNum).getReg());
  printShifter(MI, OpNum + 1, O);
}

void A64InstPrinter::printAddSubImm(const MCInst *MI, unsigned OpNum,
                                    raw_ostream &O) {
  const MCOperand &MO = MI->getOperand(OpNum);
  if (MO.isImm()) {
    unsigned Val = (MO.getImm() & 0xfff);
    assert(Val == MO.getImm() && "Add/sub immediate out of range!");
    unsigned Shift = A64_AM::getShiftValue(MI->getOperand(OpNum + 1).getImm());
    O << '#' << formatImm(Val);
    if (Shift != 0)
      printShifter(MI, OpNum + 1, O);

    if (CommentStream)
      *CommentStream << '=' << formatImm(Val << Shift) << '\n';
  } else {
    assert(MO.isExpr() && "Unexpected operand type!");
    MO.getExpr()->print(O, &MAI);
    printShifter(MI, OpNum + 1, O);
  }
}

void A64InstPrinter::printAlignedLabel(const MCInst *MI, uint64_t Address,
                                       unsigned OpNum, raw_ostream &O) {
  const MCOperand &Op = MI->getOperand(OpNum);

  // If the label has already been resolved to an immediate offset (say, when
  // we're running the disassembler), just print the immediate.
  if (Op.isImm()) {
    int64_t Offset = Op.getImm() * 4;
    if (PrintBranchImmAsAddress)
      O << formatHex(Address + Offset);
    else
      O << "#" << formatImm(Offset);
    return;
  }

  // If the branch target is simply an address then print it in hex.
  const MCConstantExpr *BranchTarget =
      dyn_cast<MCConstantExpr>(MI->getOperand(OpNum).getExpr());
  int64_t TargetAddress;
  if (BranchTarget && BranchTarget->evaluateAsAbsolute(TargetAddress)) {
    O << formatHex(TargetAddress);
  } else {
    // Otherwise, just print the expression.
    MI->getOperand(OpNum).getExpr()->print(O, &MAI);
  }
}

void A64InstPrinter::printAdrpLabel(const MCInst *MI, uint64_t Address,
                                    unsigned OpNum,
                                    raw_ostream &O) {
  const MCOperand &Op = MI->getOperand(OpNum);

  // If the label has already been resolved to an immediate offset (say, when
  // we're running the disassembler), just print the immediate.
  if (Op.isImm()) {
    const int64_t Offset = Op.getImm() * 4096;
    if (PrintBranchImmAsAddress)
      O << formatHex((Address & -4096) + Offset);
    else
      O << "#" << Offset;
    return;
  }

  // Otherwise, just print the expression.
  MI->getOperand(OpNum).getExpr()->print(O, &MAI);
}

void A64InstPrinter::printUImm12Offset(const MCInst *MI, unsigned OpNum,
                                       unsigned Scale, raw_ostream &O) {
  const MCOperand MO = MI->getOperand(OpNum);
  if (MO.isImm()) {
    O << "#" << formatImm(MO.getImm() * Scale);
  } else {
    assert(MO.isExpr() && "Unexpected operand type!");
    MO.getExpr()->print(O, &MAI);
  }
}
