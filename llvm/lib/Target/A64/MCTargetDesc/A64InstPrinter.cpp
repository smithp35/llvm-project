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

void A64InstPrinter::printShiftedRegister(const MCInst *MI, unsigned OpNum,
                                          raw_ostream &O) {
  O << getRegisterName(MI->getOperand(OpNum).getReg());
  printShifter(MI, OpNum + 1, O);
}
