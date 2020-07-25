//==- A64AsmParser.cpp - Parse A64 assembly to MCInst instructions ---------==//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
#include "MCTargetDesc/A64MCTargetDesc.h"
#include "MCTargetDesc/A64TargetStreamer.h"
#include "TargetInfo/A64TargetInfo.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCParser/MCAsmLexer.h"
#include "llvm/MC/MCParser/MCAsmParser.h"
#include "llvm/MC/MCParser/MCAsmParserExtension.h"
#include "llvm/MC/MCParser/MCParsedAsmOperand.h"
#include "llvm/MC/MCParser/MCTargetAsmParser.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/SubtargetFeature.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Casting.h"

using namespace llvm;

namespace {

class A64Operand;

class A64AsmParser : public MCTargetAsmParser {

  A64TargetStreamer &getTargetStreamer() {
    MCTargetStreamer &TS = *getParser().getStreamer().getTargetStreamer();
    return static_cast<A64TargetStreamer &>(TS);
  }

  bool MatchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                               OperandVector &Operands, MCStreamer &Out,
                               uint64_t &ErrorInfo,
                               bool MatchingInlineAsm) override;

  /// @name Auto-generated Match Functions
  /// {

#define GET_ASSEMBLER_HEADER
#include "A64GenAsmMatcher.inc"

  /// }

public:
  enum A64MatchResultTy {
    Match_InvalidSuffix = FIRST_TARGET_MATCH_RESULT_TY,
#define GET_OPERAND_DIAGNOSTIC_TYPES
#include "A64GenAsmMatcher.inc"
  };
  A64AsmParser(const MCSubtargetInfo &STI, MCAsmParser &Parser,
               const MCInstrInfo &MII, const MCTargetOptions &Options)
      : MCTargetAsmParser(Options, STI, MII) {
    MCAsmParserExtension::Initialize(Parser);
    MCStreamer &S = getParser().getStreamer();
    if (S.getTargetStreamer() == nullptr)
      new A64TargetStreamer(S);

    // Alias .hword/.word/.[dx]word to the target-independent
    // .2byte/.4byte/.8byte directives as they have the same form and
    // semantics:
    ///  ::= (.hword | .word | .dword | .xword ) [ expression (, expression)* ]
    Parser.addAliasForDirective(".hword", ".2byte");
    Parser.addAliasForDirective(".word", ".4byte");
    Parser.addAliasForDirective(".dword", ".8byte");
    Parser.addAliasForDirective(".xword", ".8byte");
  };

  SMLoc getLoc() const { return getParser().getTok().getLoc(); }

  bool ParseInstruction(ParseInstructionInfo &Info, StringRef Name,
                        SMLoc NameLoc, OperandVector &Operands) override;
  bool ParseRegister(unsigned &RegNo, SMLoc &StartLoc, SMLoc &EndLoc) override;
  bool parseOperand(OperandVector &Operands, StringRef Mnemonic);
  bool parseRegister(OperandVector &Operands);
  OperandMatchResultTy tryParseRegister(unsigned &RegNo, SMLoc &StartLoc,
                                        SMLoc &EndLoc) override;
  bool ParseDirective(AsmToken DirectiveID) override;
};

class A64Operand : public MCParsedAsmOperand {

private:
  enum KindTy {
    k_Immediate,
    k_Register,
    k_Token,
  } Kind;

  struct TokOp {
    const char *Data;
    unsigned Length;
    bool IsSuffix; // Is the operand actually a suffix on the mnemonic.
  };

  struct RegOp {
    unsigned RegNum;
  };

  struct ImmOp {
    const MCExpr *Val;
  };

  union {
    struct TokOp Tok;
    struct RegOp Reg;
    struct ImmOp Imm;
  };

  SMLoc StartLoc, EndLoc;

public:
  A64Operand(KindTy K, MCContext &Ctx) : Kind(K) {}

  bool isToken() const override { return Kind == KindTy::k_Token; }
  bool isImm() const override { return Kind == KindTy::k_Immediate; }
  bool isReg() const override { return Kind == KindTy::k_Register; }
  bool isMem() const override { return false; }

  void addRegOperands(MCInst &Inst, unsigned N) const {
    assert(N == 1 && "Invalid number of operands!");
    Inst.addOperand(MCOperand::createReg(getReg()));
  }

  void addExpr(MCInst &Inst, const MCExpr *Expr) const {
    // Add as immediates when possible.  Null MCExpr = 0.
    if (!Expr)
      Inst.addOperand(MCOperand::createImm(0));
    else if (const MCConstantExpr *CE = dyn_cast<MCConstantExpr>(Expr))
      Inst.addOperand(MCOperand::createImm(CE->getValue()));
    else
      Inst.addOperand(MCOperand::createExpr(Expr));
  }

  void addImmOperands(MCInst &Inst, unsigned N) const {
    assert(N == 1 && "Invalid number of operands!");
    addExpr(Inst, getImm());
  }

  unsigned getReg() const override {
    assert(Kind == k_Register && "Invalid access!");
    return Reg.RegNum;
  }

  const MCExpr *getImm() const {
    assert(Kind == KindTy::k_Immediate && "Invalid type access!");
    return Imm.Val;
  }

  StringRef getToken() const {
    assert(Kind == k_Token && "Invalid access!");
    return StringRef(Tok.Data, Tok.Length);
  }

  /// getStartLoc - Gets location of the first token of this operand
  SMLoc getStartLoc() const override { return StartLoc; }
  /// getEndLoc - Gets location of the last token of this operand
  SMLoc getEndLoc() const override { return EndLoc; }

  void print(raw_ostream &OS) const override {
    switch (Kind) {
    case KindTy::k_Immediate:
      OS << *getImm();
      break;
    case KindTy::k_Register:
      OS << "<register x";
      OS << getReg() << ">";
      break;
    case KindTy::k_Token:
      OS << "'" << getToken() << "'";
      break;
    }
  }

  static std::unique_ptr<A64Operand> createToken(StringRef Str, bool IsSuffix,
                                                 SMLoc S, MCContext &Ctx) {
    auto Op = std::make_unique<A64Operand>(k_Token, Ctx);
    Op->Tok.Data = Str.data();
    Op->Tok.Length = Str.size();
    Op->Tok.IsSuffix = IsSuffix;
    Op->StartLoc = S;
    Op->EndLoc = S;
    return Op;
  }

  static std::unique_ptr<A64Operand> createImm(const MCExpr *Val, SMLoc S,
                                               SMLoc E, MCContext &Ctx) {
    auto Op = std::make_unique<A64Operand>(k_Immediate, Ctx);
    Op->Imm.Val = Val;
    Op->StartLoc = S;
    Op->EndLoc = E;
    return Op;
  }

  static std::unique_ptr<A64Operand> createReg(unsigned RegNum, SMLoc S,
                                               SMLoc E, MCContext &Ctx) {
    auto Op = std::make_unique<A64Operand>(k_Register, Ctx);
    Op->Reg.RegNum = RegNum;
    Op->StartLoc = S;
    Op->EndLoc = E;
    return Op;
  }
};

} // end anonymous namespace

/// @name Auto-generated Match Functions
/// {

static unsigned MatchRegisterName(StringRef Name);

/// }

/// Try to parse a register name. The token must be an
/// Identifier when called, and if it is a register name the token is eaten and
/// the register is added to the operand list.
bool A64AsmParser::parseRegister(OperandVector &Operands) {
  MCAsmParser &Parser = getParser();
  const AsmToken &Tok = Parser.getTok();
  if (Tok.isNot(AsmToken::Identifier))
    return MatchOperand_NoMatch;

  std::string lowerCase = Tok.getString().lower();
  unsigned RegNum = MatchRegisterName(lowerCase);
  if (RegNum == 0)
    // Handle a few common aliases of registers.
    RegNum = StringSwitch<unsigned>(lowerCase)
                 .Case("fp", A64::FP)
                 .Case("lr", A64::LR)
                 .Case("x31", A64::XZR)
                 .Default(0);

  if (RegNum == 0)
    return MatchOperand_NoMatch;

  Operands.push_back(
      A64Operand::createReg(RegNum, getLoc(), getLoc(), getContext()));
  Parser.Lex(); // Eat identifier token.
  return MatchOperand_Success;
}

/// Looks at a token type and creates the relevant operand from this
/// information, adding to Operands. If operand was parsed, returns false, else
/// true.
bool A64AsmParser::parseOperand(OperandVector &Operands, StringRef Mnemonic) {
  // Check if the current operand has a custom associated parser, if so, try to
  // custom parse the operand, or fallback to the general approach.
  //  OperandMatchResultTy Result =
  //    MatchOperandParserImpl(Operands, Mnemonic,
  //    /*ParseForAllFeatures=*/true);
  // if (Result == MatchOperand_Success)
  //   return false;
  // if (Result == MatchOperand_ParseFail)
  //   return true;

  // Nothing custom, so do general case parsing.
  SMLoc S, E;
  switch (getLexer().getKind()) {
  default: {
    SMLoc S = getLoc();
    return Error(S, "invalid operand");
  }
  case AsmToken::Identifier: {
    if (parseRegister(Operands) == MatchOperand_Success)
      return false;

    // ToDo optional shift.
    return Error(S, "A64AsmParser failed to parse register as identifier");
  }
  case AsmToken::Integer:
  case AsmToken::Hash: {
    S = getLoc();

    // The # in #immediate is optional
    parseOptionalToken(AsmToken::Hash);

    const MCExpr *ImmVal;
    if (getParser().parseExpression(ImmVal))
      return true;

    E = SMLoc::getFromPointer(getLoc().getPointer() - 1);
    Operands.push_back(A64Operand::createImm(ImmVal, S, E, getContext()));
    return false;
  }
  }

  // Finally we have exhausted all options and must declare defeat.
  Error(getLoc(), "A64AsmParser::parseOperand unknown operand");
  return true;
}

/// ParseInstruction - Parse an A64 instruction mnemonic followed by its
/// operands.
/// Adapted from RISCV, liable to need expansion
bool A64AsmParser::ParseInstruction(ParseInstructionInfo &Info, StringRef Name,
                                    SMLoc NameLoc, OperandVector &Operands) {
  // First operand is token for instruction
  Operands.push_back(
      A64Operand::createToken(Name, false, NameLoc, getContext()));

  // If there are no more operands, then finish
  if (getLexer().is(AsmToken::EndOfStatement))
    return false;

  // Parse first operand
  if (parseOperand(Operands, Name))
    return true;

  // Parse until end of statement, consuming commas between operands
  unsigned OperandIdx = 1;
  while (getLexer().is(AsmToken::Comma)) {
    // Consume comma token
    getLexer().Lex();

    // Parse next operand
    if (parseOperand(Operands, Name))
      return true;

    ++OperandIdx;
  }

  if (getLexer().isNot(AsmToken::EndOfStatement)) {
    SMLoc Loc = getLexer().getLoc();
    getParser().eatToEndOfStatement();
    return Error(Loc, "unexpected token");
  }

  getParser().Lex(); // Consume the EndOfStatement.
  return false;
}

bool A64AsmParser::ParseRegister(unsigned &RegNo, SMLoc &StartLoc,
                                 SMLoc &EndLoc) {
  if (tryParseRegister(RegNo, StartLoc, EndLoc) != MatchOperand_Success)
    return Error(StartLoc, "invalid register name");
  return false;
}

OperandMatchResultTy A64AsmParser::tryParseRegister(unsigned &RegNo,
                                                    SMLoc &StartLoc,
                                                    SMLoc &EndLoc) {
  const AsmToken &Tok = getParser().getTok();
  StartLoc = Tok.getLoc();
  EndLoc = Tok.getEndLoc();
  RegNo = 0;

  // TODO more validation

  getParser().Lex(); // Eat identifier token.
  return MatchOperand_Success;

  EndLoc = SMLoc::getFromPointer(getLoc().getPointer() - 1);
  return MatchOperand_NoMatch;
}

bool A64AsmParser::ParseDirective(AsmToken DirectiveID) { return true; }

bool A64AsmParser::MatchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                                           OperandVector &Operands,
                                           MCStreamer &Out, uint64_t &ErrorInfo,
                                           bool MatchingInlineAsm) {
  MCInst Inst;

  unsigned MatchResult =
      MatchInstructionImpl(Operands, Inst, ErrorInfo, MatchingInlineAsm);

  switch (MatchResult) {
  case Match_Success:
    Inst.setLoc(IDLoc);
    Out.emitInstruction(Inst, getSTI());
    return false;
  case Match_InvalidOperand: {
    SMLoc ErrorLoc = IDLoc;

    if (ErrorInfo != ~0ULL) {
      if (ErrorInfo >= Operands.size())
        return Error(IDLoc, "too few operands for instruction",
                     SMRange(IDLoc, getTok().getLoc()));

      ErrorLoc = ((A64Operand &)*Operands[ErrorInfo]).getStartLoc();
      if (ErrorLoc == SMLoc())
        ErrorLoc = IDLoc;
    }
    return Error(ErrorLoc, "Match_InvalidOperand",
                 SMRange(IDLoc, (*Operands.back()).getEndLoc()));
  }
  case Match_MnemonicFail:
    return Error(IDLoc, "Mnemonic Fail", SMRange(IDLoc, getTok().getLoc()));
  case Match_InvalidTiedOperand:
    return Error(IDLoc, "Invalid Tied Operand",
                 SMRange(IDLoc, getTok().getLoc()));
  case Match_MissingFeature:
    return Error(IDLoc, "Missing Feature", SMRange(IDLoc, getTok().getLoc()));
  default:
    return Error(IDLoc, "Unknown instruction match error",
                 SMRange(IDLoc, getTok().getLoc()));
  }
  return true;
}

/// Force static initialization.
extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeA64AsmParser() {
  RegisterMCAsmParser<A64AsmParser> X(getTheA64Target());
}

#define GET_REGISTER_MATCHER
#define GET_MATCHER_IMPLEMENTATION
#include "A64GenAsmMatcher.inc"
