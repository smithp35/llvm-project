//==- A64AsmParser.cpp - Parse A64 assembly to MCInst instructions ---------==//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
#include "MCTargetDesc/A64AddressingModes.h"
#include "MCTargetDesc/A64MCExpr.h"
#include "MCTargetDesc/A64MCTargetDesc.h"
#include "MCTargetDesc/A64TargetStreamer.h"
#include "TargetInfo/A64TargetInfo.h"
#include "Utils/A64BaseInfo.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCParser/MCAsmLexer.h"
#include "llvm/MC/MCParser/MCAsmParser.h"
#include "llvm/MC/MCParser/MCAsmParserExtension.h"
#include "llvm/MC/MCParser/MCParsedAsmOperand.h"
#include "llvm/MC/MCParser/MCTargetAsmParser.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCValue.h"
#include "llvm/MC/SubtargetFeature.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Casting.h"

using namespace llvm;

namespace {

class A64Operand;

class A64AsmParser : public MCTargetAsmParser {
private:
  StringRef Mnemonic; ///< Instruction mnemonic.

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
  A64CC::CondCode parseCondCodeString(StringRef Cond);
  bool parseCondCode(OperandVector &Operands);
  bool ParseInstruction(ParseInstructionInfo &Info, StringRef Name,
                        SMLoc NameLoc, OperandVector &Operands) override;
  bool ParseRegister(unsigned &RegNo, SMLoc &StartLoc, SMLoc &EndLoc) override;
  bool parseOperand(OperandVector &Operands, StringRef Mnemonic,
                    bool isCondCode);
  bool parseRegister(OperandVector &Operands);
  bool parseSymbolicImmVal(const MCExpr *&ImmVal);
  OperandMatchResultTy tryParseAdrLabel(OperandVector &Operands);
  OperandMatchResultTy tryParseAdrpLabel(OperandVector &Operands);
  OperandMatchResultTy tryParseRegister(unsigned &RegNo, SMLoc &StartLoc,
                                        SMLoc &EndLoc) override;
  OperandMatchResultTy tryParseOptionalShiftExtend(OperandVector &Operands);
  bool ParseDirective(AsmToken DirectiveID) override;
  OperandMatchResultTy tryParseImmWithOptionalShift(OperandVector &Operands);

  static bool classifySymbolRef(const MCExpr *Expr,
                                A64MCExpr::VariantKind &ELFRefKind,
                                MCSymbolRefExpr::VariantKind &DarwinRefKind,
                                int64_t &Addend);
};

class A64Operand : public MCParsedAsmOperand {

private:
  enum KindTy {
    k_Immediate,
    k_ShiftedImm,
    k_CondCode,
    k_Register,
    k_ShiftExtend,
    k_Token,
  } Kind;

  struct TokOp {
    const char *Data;
    unsigned Length;
    bool IsSuffix; // Is the operand actually a suffix on the mnemonic.
  };

  // Separate shift/extend operand.
  struct ShiftExtendOp {
    A64_AM::ShiftExtendType Type;
    unsigned Amount;
    bool HasExplicitAmount;
  };

  struct RegOp {
    unsigned RegNum;

    // In some cases the shift/extend needs to be explicitly parsed together
    // with the register, rather than as a separate operand. This is needed
    // for addressing modes where the instruction as a whole dictates the
    // scaling/extend, rather than specific bits in the instruction.
    // By parsing them as a single operand, we avoid the need to pass an
    // extra operand in all CodeGen patterns (because all operands need to
    // have an associated value), and we avoid the need to update TableGen to
    // accept operands that have no associated bits in the instruction.
    //
    // An added benefit of parsing them together is that the assembler
    // can give a sensible diagnostic if the scaling is not correct.
    //
    // The default is 'lsl #0' (HasExplicitAmount = false) if no
    // ShiftExtend is specified.
    ShiftExtendOp ShiftExtend;
  };

  struct ImmOp {
    const MCExpr *Val;
  };

  struct ShiftedImmOp {
    const MCExpr *Val;
    unsigned ShiftAmount;
  };

  struct CondCodeOp {
    A64CC::CondCode Code;
  };

  union {
    struct TokOp Tok;
    struct RegOp Reg;
    struct ImmOp Imm;
    struct ShiftedImmOp ShiftedImm;
    struct ShiftExtendOp ShiftExtend;
    struct CondCodeOp CondCode;
  };

  SMLoc StartLoc, EndLoc;

public:
  A64Operand(KindTy K, MCContext &Ctx) : Kind(K) {}
  A64Operand(const A64Operand &o) : MCParsedAsmOperand() {
    Kind = o.Kind;
    StartLoc = o.StartLoc;
    EndLoc = o.EndLoc;
    switch (Kind) {
    case k_Token:
      Tok = o.Tok;
      break;
    case k_Immediate:
      Imm = o.Imm;
      break;
    case k_CondCode:
      CondCode = o.CondCode;
      break;
    case k_Register:
      Reg = o.Reg;
      break;
    case k_ShiftExtend:
      ShiftExtend = o.ShiftExtend;
      break;
    case k_ShiftedImm:
      ShiftedImm = o.ShiftedImm;
      break;
    }
  }

  A64_AM::ShiftExtendType getShiftExtendType() const {
    if (Kind == k_ShiftExtend)
      return ShiftExtend.Type;
    if (Kind == k_Register)
      return Reg.ShiftExtend.Type;
    llvm_unreachable("Invalid access!");
  }

  unsigned getShiftExtendAmount() const {
    if (Kind == k_ShiftExtend)
      return ShiftExtend.Amount;
    if (Kind == k_Register)
      return Reg.ShiftExtend.Amount;
    llvm_unreachable("Invalid access!");
  }

  A64CC::CondCode getCondCode() const {
    assert(Kind == k_CondCode && "Invalid access!");
    return CondCode.Code;
  }

  bool hasShiftExtendAmount() const {
    if (Kind == k_ShiftExtend)
      return ShiftExtend.HasExplicitAmount;
    if (Kind == k_Register)
      return Reg.ShiftExtend.HasExplicitAmount;
    llvm_unreachable("Invalid access!");
  }

  bool isToken() const override { return Kind == KindTy::k_Token; }
  bool isImm() const override { return Kind == KindTy::k_Immediate; }
  bool isReg() const override { return Kind == KindTy::k_Register; }
  bool isMem() const override { return false; }
  bool isShiftExtend() const { return Kind == k_ShiftExtend; }
  bool isShiftedImm() const { return Kind == k_ShiftedImm; }
  bool isShifter() const {
    if (!isShiftExtend())
      return false;

    A64_AM::ShiftExtendType ST = getShiftExtendType();
    return (ST == A64_AM::LSL || ST == A64_AM::LSR || ST == A64_AM::ASR ||
            ST == A64_AM::ROR || ST == A64_AM::MSL);
  }

  bool isAdrLabel() const {
    // Validation was handled during parsing, so we just sanity check that
    // something didn't go haywire.
    if (!isImm())
      return false;

    if (const MCConstantExpr *CE = dyn_cast<MCConstantExpr>(Imm.Val)) {
      int64_t Val = CE->getValue();
      int64_t Min = -(1LL << (21 - 1));
      int64_t Max = ((1LL << (21 - 1)) - 1);
      return Val >= Min && Val <= Max;
    }

    return true;
  }

  bool isAdrpLabel() const {
    // Validation was handled during parsing, so we just sanity check that
    // something didn't go haywire.
    if (!isImm())
      return false;

    if (const MCConstantExpr *CE = dyn_cast<MCConstantExpr>(Imm.Val)) {
      int64_t Val = CE->getValue();
      int64_t Min = -(4096 * (1LL << (21 - 1)));
      int64_t Max = 4096 * ((1LL << (21 - 1)) - 1);
      return (Val % 4096) == 0 && Val >= Min && Val <= Max;
    }

    return true;
  }

  bool isArithmeticShifter() const {
    if (!isShifter())
      return false;

    // An arithmetic shifter is LSL, LSR, or ASR.
    A64_AM::ShiftExtendType ST = getShiftExtendType();
    return (ST == A64_AM::LSL || ST == A64_AM::LSR || ST == A64_AM::ASR) &&
           getShiftExtendAmount() < 64;
  }

  bool isLogicalShifter() const {
    if (!isShifter())
      return false;

    // A logical shifter is LSL, LSR, ASR or ROR.
    A64_AM::ShiftExtendType ST = getShiftExtendType();
    return (ST == A64_AM::LSL || ST == A64_AM::LSR || ST == A64_AM::ASR ||
            ST == A64_AM::ROR) &&
           getShiftExtendAmount() < 64;
  }

  bool isMovImm64Shifter() const {
    if (!isShifter())
      return false;

    // A MOVi shifter is LSL of 0 or 16.
    A64_AM::ShiftExtendType ST = getShiftExtendType();
    if (ST != A64_AM::LSL)
      return false;
    uint64_t Val = getShiftExtendAmount();
    return (Val == 0 || Val == 16 || Val == 32 || Val == 48);
  }

  bool isMovWSymbol(ArrayRef<A64MCExpr::VariantKind> AllowedModifiers) const {
    if (!isImm())
      return false;

    A64MCExpr::VariantKind ELFRefKind;
    MCSymbolRefExpr::VariantKind DarwinRefKind;
    int64_t Addend;
    if (!A64AsmParser::classifySymbolRef(getImm(), ELFRefKind, DarwinRefKind,
                                         Addend)) {
      return false;
    }
    if (DarwinRefKind != MCSymbolRefExpr::VK_None)
      return false;

    for (unsigned i = 0; i != AllowedModifiers.size(); ++i) {
      if (ELFRefKind == AllowedModifiers[i])
        return true;
    }

    return false;
  }

  bool isMovWSymbolG3() const { return isMovWSymbol({A64MCExpr::VK_ABS_G3}); }

  bool isMovWSymbolG2() const {
    return isMovWSymbol({A64MCExpr::VK_ABS_G2, A64MCExpr::VK_ABS_G2_S,
                         A64MCExpr::VK_ABS_G2_NC});
  }

  bool isMovWSymbolG1() const {
    return isMovWSymbol({A64MCExpr::VK_ABS_G1, A64MCExpr::VK_ABS_G1_S,
                         A64MCExpr::VK_ABS_G1_NC});
  }

  bool isMovWSymbolG0() const {
    return isMovWSymbol({A64MCExpr::VK_ABS_G0, A64MCExpr::VK_ABS_G0_S,
                         A64MCExpr::VK_ABS_G0_NC});
  }

  bool isCondCode() const { return Kind == k_CondCode; }

  template <int N> bool isBranchTarget() const {
    if (!isImm())
      return false;
    const MCConstantExpr *MCE = dyn_cast<MCConstantExpr>(getImm());
    if (!MCE)
      return true;
    int64_t Val = MCE->getValue();
    if (Val & 0x3)
      return false;
    assert(N > 0 && "Branch target immediate cannot be 0 bits!");
    return (Val >= -((1 << (N - 1)) << 2) &&
            Val <= (((1 << (N - 1)) - 1) << 2));
  }

  /// Returns the immediate value as a pair of (imm, shift) if the immediate is
  /// a shifted immediate by value 'Shift' or '0', or if it is an unshifted
  /// immediate that can be shifted by 'Shift'.
  template <unsigned Width>
  Optional<std::pair<int64_t, unsigned>> getShiftedVal() const {
    if (isShiftedImm() && Width == getShiftedImmShift())
      if (auto *CE = dyn_cast<MCConstantExpr>(getShiftedImmVal()))
        return std::make_pair(CE->getValue(), Width);

    if (isImm())
      if (auto *CE = dyn_cast<MCConstantExpr>(getImm())) {
        int64_t Val = CE->getValue();
        if ((Val != 0) && (uint64_t(Val >> Width) << Width) == uint64_t(Val))
          return std::make_pair(Val >> Width, Width);
        else
          return std::make_pair(Val, 0u);
      }
    return {};
  }

  bool isAddSubImm() const {
    if (!isShiftedImm() && !isImm())
      return false;

    const MCExpr *Expr;

    // An ADD/SUB shifter is either 'lsl #0' or 'lsl #12'.
    if (isShiftedImm()) {
      unsigned Shift = ShiftedImm.ShiftAmount;
      Expr = ShiftedImm.Val;
      if (Shift != 0 && Shift != 12)
        return false;
    } else {
      Expr = getImm();
    }

    A64MCExpr::VariantKind ELFRefKind;
    MCSymbolRefExpr::VariantKind DarwinRefKind;
    int64_t Addend;
    if (A64AsmParser::classifySymbolRef(Expr, ELFRefKind, DarwinRefKind,
                                        Addend)) {
      return ELFRefKind == A64MCExpr::VK_LO12;
    }

    // If it's a constant, it should be a real immediate in range.
    if (auto ShiftedVal = getShiftedVal<12>())
      return ShiftedVal->first >= 0 && ShiftedVal->first <= 0xfff;

    // If it's an expression, we hope for the best and let the fixup/relocation
    // code deal with it.
    return true;
  }

  bool isAddSubImmNeg() const {
    if (!isShiftedImm() && !isImm())
      return false;

    // Otherwise it should be a real negative immediate in range.
    if (auto ShiftedVal = getShiftedVal<12>())
      return ShiftedVal->first < 0 && -ShiftedVal->first <= 0xfff;

    return false;
  }

  template <int N, int M> bool isImmInRange() const {
    if (!isImm())
      return false;
    const MCConstantExpr *MCE = dyn_cast<MCConstantExpr>(getImm());
    if (!MCE)
      return false;
    int64_t Val = MCE->getValue();
    return (Val >= N && Val <= M);
  }

  bool isSymbolicUImm12Offset(const MCExpr *Expr) const {
    A64MCExpr::VariantKind ELFRefKind;
    MCSymbolRefExpr::VariantKind DarwinRefKind;
    int64_t Addend;
    if (!A64AsmParser::classifySymbolRef(Expr, ELFRefKind, DarwinRefKind,
                                         Addend)) {
      // If we don't understand the expression, assume the best and
      // let the fixup and relocation code deal with it.
      return true;
    }

    if (DarwinRefKind == MCSymbolRefExpr::VK_PAGEOFF ||
        ELFRefKind == A64MCExpr::VK_LO12)
      return true;

    return false;
  }

  template <int Scale> bool isUImm12Offset() const {
    if (!isImm())
      return false;

    const MCConstantExpr *MCE = dyn_cast<MCConstantExpr>(getImm());
    if (!MCE)
      return isSymbolicUImm12Offset(getImm());

    int64_t Val = MCE->getValue();
    return (Val % Scale) == 0 && Val >= 0 && (Val / Scale) < 0x1000;
  }

  // NOTE: Also used for isLogicalImmNot as anything that can be represented as
  // a logical immediate can always be represented when inverted.
  template <typename T> bool isLogicalImm() const {
    if (!isImm())
      return false;
    const MCConstantExpr *MCE = dyn_cast<MCConstantExpr>(getImm());
    if (!MCE)
      return false;

    int64_t Val = MCE->getValue();
    // Avoid left shift by 64 directly.
    uint64_t Upper = UINT64_C(-1) << (sizeof(T) * 4) << (sizeof(T) * 4);
    // Allow all-0 or all-1 in top bits to permit bitwise NOT.
    if ((Val & Upper) && (Val & Upper) != Upper)
      return false;

    return A64_AM::isLogicalImmediate(Val & ~Upper, sizeof(T) * 8);
  }

  void addCondCodeOperands(MCInst &Inst, unsigned N) const {
    assert(N == 1 && "Invalid number of operands!");
    Inst.addOperand(MCOperand::createImm(getCondCode()));
  }

  void addAdrLabelOperands(MCInst &Inst, unsigned N) const {
    addImmOperands(Inst, N);
  }

  void addAdrpLabelOperands(MCInst &Inst, unsigned N) const {
    assert(N == 1 && "Invalid number of operands!");
    const MCConstantExpr *MCE = dyn_cast<MCConstantExpr>(getImm());
    if (!MCE)
      addExpr(Inst, getImm());
    else
      Inst.addOperand(MCOperand::createImm(MCE->getValue() >> 12));
  }

  void addBranchTarget26Operands(MCInst &Inst, unsigned N) const {
    // Branch operands don't encode the low bits, so shift them off
    // here. If it's a label, however, just put it on directly as there's
    // not enough information now to do anything.
    assert(N == 1 && "Invalid number of operands!");
    const MCConstantExpr *MCE = dyn_cast<MCConstantExpr>(getImm());
    if (!MCE) {
      addExpr(Inst, getImm());
      return;
    }
    assert(MCE && "Invalid constant immediate operand!");
    Inst.addOperand(MCOperand::createImm(MCE->getValue() >> 2));
  }

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

  void addShifterOperands(MCInst &Inst, unsigned N) const {
    assert(N == 1 && "Invalid number of operands!");
    unsigned Imm =
        A64_AM::getShifterImm(getShiftExtendType(), getShiftExtendAmount());
    Inst.addOperand(MCOperand::createImm(Imm));
  }

  template <int Shift>
  void addImmWithOptionalShiftOperands(MCInst &Inst, unsigned N) const {
    assert(N == 2 && "Invalid number of operands!");
    if (auto ShiftedVal = getShiftedVal<Shift>()) {
      Inst.addOperand(MCOperand::createImm(ShiftedVal->first));
      Inst.addOperand(MCOperand::createImm(ShiftedVal->second));
    } else if (isShiftedImm()) {
      addExpr(Inst, getShiftedImmVal());
      Inst.addOperand(MCOperand::createImm(getShiftedImmShift()));
    } else {
      addExpr(Inst, getImm());
      Inst.addOperand(MCOperand::createImm(0));
    }
  }

  template <typename T>
  void addLogicalImmOperands(MCInst &Inst, unsigned N) const {
    assert(N == 1 && "Invalid number of operands!");
    const MCConstantExpr *MCE = cast<MCConstantExpr>(getImm());
    std::make_unsigned_t<T> Val = MCE->getValue();
    uint64_t encoding = A64_AM::encodeLogicalImmediate(Val, sizeof(T) * 8);
    Inst.addOperand(MCOperand::createImm(encoding));
  }

  template <typename T>
  void addLogicalImmNotOperands(MCInst &Inst, unsigned N) const {
    assert(N == 1 && "Invalid number of operands!");
    const MCConstantExpr *MCE = cast<MCConstantExpr>(getImm());
    std::make_unsigned_t<T> Val = ~MCE->getValue();
    uint64_t encoding = A64_AM::encodeLogicalImmediate(Val, sizeof(T) * 8);
    Inst.addOperand(MCOperand::createImm(encoding));
  }

  void addPCRelLabel19Operands(MCInst &Inst, unsigned N) const {
    // Branch operands don't encode the low bits, so shift them off
    // here. If it's a label, however, just put it on directly as there's
    // not enough information now to do anything.
    assert(N == 1 && "Invalid number of operands!");
    const MCConstantExpr *MCE = dyn_cast<MCConstantExpr>(getImm());
    if (!MCE) {
      addExpr(Inst, getImm());
      return;
    }
    assert(MCE && "Invalid constant immediate operand!");
    Inst.addOperand(MCOperand::createImm(MCE->getValue() >> 2));
  }

  template <int Scale>
  void addUImm12OffsetOperands(MCInst &Inst, unsigned N) const {
    assert(N == 1 && "Invalid number of operands!");
    const MCConstantExpr *MCE = dyn_cast<MCConstantExpr>(getImm());

    if (!MCE) {
      Inst.addOperand(MCOperand::createExpr(getImm()));
      return;
    }
    Inst.addOperand(MCOperand::createImm(MCE->getValue() / Scale));
  }

  unsigned getReg() const override {
    assert(Kind == k_Register && "Invalid access!");
    return Reg.RegNum;
  }

  const MCExpr *getImm() const {
    assert(Kind == KindTy::k_Immediate && "Invalid type access!");
    return Imm.Val;
  }

  const MCExpr *getShiftedImmVal() const {
    assert(Kind == k_ShiftedImm && "Invalid access!");
    return ShiftedImm.Val;
  }

  unsigned getShiftedImmShift() const {
    assert(Kind == k_ShiftedImm && "Invalid access!");
    return ShiftedImm.ShiftAmount;
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
    case k_CondCode:
      OS << "<condcode " << getCondCode() << ">";
      break;
    case KindTy::k_Register:
      OS << "<register x";
      OS << getReg() << ">";
      if (!getShiftExtendAmount() && !hasShiftExtendAmount())
        break;
      LLVM_FALLTHROUGH;
    case k_ShiftedImm: {
      unsigned Shift = getShiftedImmShift();
      OS << "<shiftedimm ";
      OS << *getShiftedImmVal();
      OS << ", lsl #" << A64_AM::getShiftValue(Shift) << ">";
      break;
    }
    case k_ShiftExtend:
      OS << "<" << A64_AM::getShiftExtendName(getShiftExtendType()) << " #"
         << getShiftExtendAmount();
      if (!hasShiftExtendAmount())
        OS << "<imp>";
      OS << '>';
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

  static std::unique_ptr<A64Operand>
  createReg(unsigned RegNum, SMLoc S, SMLoc E, MCContext &Ctx,
            A64_AM::ShiftExtendType ExtTy = A64_AM::LSL,
            unsigned ShiftAmount = 0, unsigned HasExplicitAmount = false) {
    auto Op = std::make_unique<A64Operand>(k_Register, Ctx);
    Op->Reg.RegNum = RegNum;
    Op->Reg.ShiftExtend.Type = ExtTy;
    Op->Reg.ShiftExtend.Amount = ShiftAmount;
    Op->Reg.ShiftExtend.HasExplicitAmount = HasExplicitAmount;
    Op->StartLoc = S;
    Op->EndLoc = E;
    return Op;
  }

  static std::unique_ptr<A64Operand>
  CreateShiftExtend(A64_AM::ShiftExtendType ShOp, unsigned Val,
                    bool HasExplicitAmount, SMLoc S, SMLoc E, MCContext &Ctx) {
    auto Op = std::make_unique<A64Operand>(k_ShiftExtend, Ctx);
    Op->ShiftExtend.Type = ShOp;
    Op->ShiftExtend.Amount = Val;
    Op->ShiftExtend.HasExplicitAmount = HasExplicitAmount;
    Op->StartLoc = S;
    Op->EndLoc = E;
    return Op;
  }

  static std::unique_ptr<A64Operand> CreateImm(const MCExpr *Val, SMLoc S,
                                               SMLoc E, MCContext &Ctx) {
    auto Op = std::make_unique<A64Operand>(k_Immediate, Ctx);
    Op->Imm.Val = Val;
    Op->StartLoc = S;
    Op->EndLoc = E;
    return Op;
  }

  static std::unique_ptr<A64Operand> CreateShiftedImm(const MCExpr *Val,
                                                      unsigned ShiftAmount,
                                                      SMLoc S, SMLoc E,
                                                      MCContext &Ctx) {
    auto Op = std::make_unique<A64Operand>(k_ShiftedImm, Ctx);
    Op->ShiftedImm.Val = Val;
    Op->ShiftedImm.ShiftAmount = ShiftAmount;
    Op->StartLoc = S;
    Op->EndLoc = E;
    return Op;
  }

  static std::unique_ptr<A64Operand>
  CreateCondCode(A64CC::CondCode Code, SMLoc S, SMLoc E, MCContext &Ctx) {
    auto Op = std::make_unique<A64Operand>(k_CondCode, Ctx);
    Op->CondCode.Code = Code;
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

/// parseCondCodeString - Parse a Condition Code string.
A64CC::CondCode A64AsmParser::parseCondCodeString(StringRef Cond) {
  A64CC::CondCode CC = StringSwitch<A64CC::CondCode>(Cond.lower())
                           .Case("eq", A64CC::EQ)
                           .Case("ne", A64CC::NE)
                           .Case("cs", A64CC::HS)
                           .Case("hs", A64CC::HS)
                           .Case("cc", A64CC::LO)
                           .Case("lo", A64CC::LO)
                           .Case("mi", A64CC::MI)
                           .Case("pl", A64CC::PL)
                           .Case("vs", A64CC::VS)
                           .Case("vc", A64CC::VC)
                           .Case("hi", A64CC::HI)
                           .Case("ls", A64CC::LS)
                           .Case("ge", A64CC::GE)
                           .Case("lt", A64CC::LT)
                           .Case("gt", A64CC::GT)
                           .Case("le", A64CC::LE)
                           .Case("al", A64CC::AL)
                           .Case("nv", A64CC::NV)
                           .Default(A64CC::Invalid);
  return CC;
}

/// parseCondCode - Parse a Condition Code operand.
bool A64AsmParser::parseCondCode(OperandVector &Operands) {
  MCAsmParser &Parser = getParser();
  SMLoc S = getLoc();
  const AsmToken &Tok = Parser.getTok();
  assert(Tok.is(AsmToken::Identifier) && "Token is not an Identifier");

  StringRef Cond = Tok.getString();
  A64CC::CondCode CC = parseCondCodeString(Cond);
  if (CC == A64CC::Invalid)
    return TokError("invalid condition code");
  Parser.Lex(); // Eat identifier token.

  Operands.push_back(A64Operand::CreateCondCode(CC, S, getLoc(), getContext()));
  return false;
}

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
bool A64AsmParser::parseOperand(OperandVector &Operands, StringRef Mnemonic,
                                bool isCondCode) {
  // Check if the current operand has a custom associated parser, if so, try to
  // custom parse the operand, or fallback to the general approach.
  OperandMatchResultTy Result =
      MatchOperandParserImpl(Operands, Mnemonic,
                             /*ParseForAllFeatures=*/true);
  if (Result == MatchOperand_Success)
    return false;
  if (Result == MatchOperand_ParseFail)
    return true;
  // Nothing custom, so do general case parsing.
  SMLoc S, E;
  MCAsmParser &Parser = getParser();
  switch (getLexer().getKind()) {
  default: {
    SMLoc S = getLoc();
    const MCExpr *Expr;
    if (parseSymbolicImmVal(Expr))
      return Error(S, "invalid operand");
    SMLoc E = SMLoc::getFromPointer(getLoc().getPointer() - 1);
    Operands.push_back(A64Operand::CreateImm(Expr, S, E, getContext()));
    return false;
  }
  case AsmToken::Identifier: {

    // If we're expecting a Condition Code operand, then just parse that.
    if (isCondCode)
      return parseCondCode(Operands);

    if (parseRegister(Operands) == MatchOperand_Success)
      return false;

    // This could be an optional "shift" or "extend" operand.
    OperandMatchResultTy GotShift = tryParseOptionalShiftExtend(Operands);
    // We can only continue if no tokens were eaten.
    if (GotShift != MatchOperand_NoMatch)
      return GotShift;

    // This was not a register so parse other operands that start with an
    // identifier (like labels) as expressions and create them as immediates.
    const MCExpr *IdVal;
    S = getLoc();
    if (getParser().parseExpression(IdVal))
      return true;
    E = SMLoc::getFromPointer(getLoc().getPointer() - 1);
    Operands.push_back(A64Operand::CreateImm(IdVal, S, E, getContext()));
    return false;
  }
  case AsmToken::LBrac: {
    SMLoc Loc = Parser.getTok().getLoc();
    Operands.push_back(A64Operand::createToken("[", false, Loc, getContext()));
    Parser.Lex(); // Eat '['

    // There's no comma after a '[', so we can parse the next operand
    // immediately.
    return parseOperand(Operands, Mnemonic, /* isCondCode */ false);
  }
  case AsmToken::Integer:
  case AsmToken::Hash: {
    S = getLoc();

    // The # in #immediate is optional
    parseOptionalToken(AsmToken::Hash);

    const MCExpr *ImmVal;
    if (parseSymbolicImmVal(ImmVal))
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
  MCAsmParser &Parser = getParser();
  // Common alias for conditional branches
  Name = StringSwitch<StringRef>(Name.lower())
             .Case("beq", "b.eq")
             .Case("bne", "b.ne")
             .Case("bhs", "b.hs")
             .Case("bcs", "b.cs")
             .Case("blo", "b.lo")
             .Case("bcc", "b.cc")
             .Case("bmi", "b.mi")
             .Case("bpl", "b.pl")
             .Case("bvs", "b.vs")
             .Case("bvc", "b.vc")
             .Case("bhi", "b.hi")
             .Case("bls", "b.ls")
             .Case("bge", "b.ge")
             .Case("blt", "b.lt")
             .Case("bgt", "b.gt")
             .Case("ble", "b.le")
             .Case("bal", "b.al")
             .Case("bnv", "b.nv")
             .Default(Name);

  // Create the leading tokens for the mnemonic, split by '.' characters.
  size_t Start = 0, Next = Name.find('.');
  StringRef Head = Name.slice(Start, Next);
  StringRef Mnemonic = Head;

  // First operand is token for instruction
  Operands.push_back(
      A64Operand::createToken(Head, false, NameLoc, getContext()));

  // Handle condition codes for a branch mnemonic
  if (Head == "b" && Next != StringRef::npos) {
    Start = Next;
    Next = Name.find('.', Start + 1);
    Head = Name.slice(Start + 1, Next);
    SMLoc SuffixLoc = SMLoc::getFromPointer(NameLoc.getPointer() +
                                            (Head.data() - Name.data()));
    A64CC::CondCode CC = parseCondCodeString(Head);
    if (CC == A64CC::Invalid)
      return Error(SuffixLoc, "invalid condition code");
    Operands.push_back(
        A64Operand::createToken(".", true, SuffixLoc, getContext()));
    Operands.push_back(
        A64Operand::CreateCondCode(CC, NameLoc, NameLoc, getContext()));
  }

  // If there are no more operands, then finish
  if (getLexer().is(AsmToken::EndOfStatement))
    return false;

  // Conditional compare instructions have a Condition Code operand, which needs
  // to be parsed and an immediate operand created.
  bool condCodeFourthOperand =
      (Head == "ccmp" || Head == "ccmn" || Head == "csel");

  // Parse first operand
  if (parseOperand(Operands, Mnemonic, /* isCondCode */ false))
    return true;

  // Parse until end of statement, consuming commas between operands
  unsigned OperandIdx = 1;
  while (getLexer().is(AsmToken::Comma)) {
    // Consume comma token
    getLexer().Lex();

    // Parse next operand
    if (parseOperand(Operands, Mnemonic,
                     OperandIdx == 3 && condCodeFourthOperand))
      return true;

    // Handle close square brackets
    SMLoc RLoc = Parser.getTok().getLoc();
    if (parseOptionalToken(AsmToken::RBrac))
      Operands.push_back(
          A64Operand::createToken("]", false, RLoc, getContext()));

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

/// tryParseOptionalShift - Some operands take an optional shift argument. Parse
/// them if present.
OperandMatchResultTy
A64AsmParser::tryParseOptionalShiftExtend(OperandVector &Operands) {
  MCAsmParser &Parser = getParser();
  const AsmToken &Tok = Parser.getTok();
  std::string LowerID = Tok.getString().lower();
  A64_AM::ShiftExtendType ShOp = StringSwitch<A64_AM::ShiftExtendType>(LowerID)
                                     .Case("lsl", A64_AM::LSL)
                                     .Case("lsr", A64_AM::LSR)
                                     .Case("asr", A64_AM::ASR)
                                     .Case("ror", A64_AM::ROR)
                                     .Case("msl", A64_AM::MSL)
                                     .Case("uxtb", A64_AM::UXTB)
                                     .Case("uxth", A64_AM::UXTH)
                                     .Case("uxtw", A64_AM::UXTW)
                                     .Case("uxtx", A64_AM::UXTX)
                                     .Case("sxtb", A64_AM::SXTB)
                                     .Case("sxth", A64_AM::SXTH)
                                     .Case("sxtw", A64_AM::SXTW)
                                     .Case("sxtx", A64_AM::SXTX)
                                     .Default(A64_AM::InvalidShiftExtend);

  if (ShOp == A64_AM::InvalidShiftExtend)
    return MatchOperand_NoMatch;

  SMLoc S = Tok.getLoc();
  Parser.Lex();

  bool Hash = parseOptionalToken(AsmToken::Hash);

  if (!Hash && getLexer().isNot(AsmToken::Integer)) {
    if (ShOp == A64_AM::LSL || ShOp == A64_AM::LSR || ShOp == A64_AM::ASR ||
        ShOp == A64_AM::ROR || ShOp == A64_AM::MSL) {
      // We expect a number here.
      TokError("expected #imm after shift specifier");
      return MatchOperand_ParseFail;
    }

    // "extend" type operations don't need an immediate, #0 is implicit.
    SMLoc E = SMLoc::getFromPointer(getLoc().getPointer() - 1);
    Operands.push_back(
        A64Operand::CreateShiftExtend(ShOp, 0, false, S, E, getContext()));
    return MatchOperand_Success;
  }

  // Make sure we do actually have a number, identifier or a parenthesized
  // expression.
  SMLoc E = Parser.getTok().getLoc();
  if (!Parser.getTok().is(AsmToken::Integer) &&
      !Parser.getTok().is(AsmToken::LParen) &&
      !Parser.getTok().is(AsmToken::Identifier)) {
    Error(E, "expected integer shift amount");
    return MatchOperand_ParseFail;
  }

  const MCExpr *ImmVal;
  if (getParser().parseExpression(ImmVal))
    return MatchOperand_ParseFail;

  const MCConstantExpr *MCE = dyn_cast<MCConstantExpr>(ImmVal);
  if (!MCE) {
    Error(E, "expected constant '#imm' after shift specifier");
    return MatchOperand_ParseFail;
  }

  E = SMLoc::getFromPointer(getLoc().getPointer() - 1);
  Operands.push_back(A64Operand::CreateShiftExtend(ShOp, MCE->getValue(), true,
                                                   S, E, getContext()));
  return MatchOperand_Success;
}

/// tryParseImmWithOptionalShift - Parse immediate operand, optionally with
/// a shift suffix, for example '#1, lsl #12'.
OperandMatchResultTy
A64AsmParser::tryParseImmWithOptionalShift(OperandVector &Operands) {
  MCAsmParser &Parser = getParser();
  SMLoc S = getLoc();

  if (Parser.getTok().is(AsmToken::Hash))
    Parser.Lex(); // Eat '#'
  else if (Parser.getTok().isNot(AsmToken::Integer))
    // Operand should start from # or should be integer, emit error otherwise.
    return MatchOperand_NoMatch;

  const MCExpr *Imm = nullptr;
  // TODO support symbolic immediates
  if (Parser.parseExpression(Imm))
    return MatchOperand_ParseFail;
  else if (Parser.getTok().isNot(AsmToken::Comma)) {
    SMLoc E = Parser.getTok().getLoc();
    Operands.push_back(A64Operand::CreateImm(Imm, S, E, getContext()));
    return MatchOperand_Success;
  }
  // Eat ','
  Parser.Lex();

  // The optional operand must be "lsl #N" where N is non-negative.
  if (!Parser.getTok().is(AsmToken::Identifier) ||
      !Parser.getTok().getIdentifier().equals_insensitive("lsl")) {
    Error(Parser.getTok().getLoc(), "only 'lsl #+N' valid after immediate");
    return MatchOperand_ParseFail;
  }

  // Eat 'lsl'
  Parser.Lex();

  parseOptionalToken(AsmToken::Hash);

  if (Parser.getTok().isNot(AsmToken::Integer)) {
    Error(Parser.getTok().getLoc(), "only 'lsl #+N' valid after immediate");
    return MatchOperand_ParseFail;
  }

  int64_t ShiftAmount = Parser.getTok().getIntVal();

  if (ShiftAmount < 0) {
    Error(Parser.getTok().getLoc(), "positive shift amount required");
    return MatchOperand_ParseFail;
  }
  Parser.Lex(); // Eat the number

  // Just in case the optional lsl #0 is used for immediates other than zero.
  if (ShiftAmount == 0 && Imm != nullptr) {
    SMLoc E = Parser.getTok().getLoc();
    Operands.push_back(A64Operand::CreateImm(Imm, S, E, getContext()));
    return MatchOperand_Success;
  }

  SMLoc E = Parser.getTok().getLoc();
  Operands.push_back(
      A64Operand::CreateShiftedImm(Imm, ShiftAmount, S, E, getContext()));
  return MatchOperand_Success;
}

bool A64AsmParser::parseSymbolicImmVal(const MCExpr *&ImmVal) {
  MCAsmParser &Parser = getParser();
  bool HasELFModifier = false;
  A64MCExpr::VariantKind RefKind;

  if (parseOptionalToken(AsmToken::Colon)) {
    HasELFModifier = true;

    if (Parser.getTok().isNot(AsmToken::Identifier))
      return TokError("expect relocation specifier in operand after ':'");

    std::string LowerCase = Parser.getTok().getIdentifier().lower();
    RefKind = StringSwitch<A64MCExpr::VariantKind>(LowerCase)
                  .Case("lo12", A64MCExpr::VK_LO12)
                  .Case("abs_g3", A64MCExpr::VK_ABS_G3)
                  .Case("abs_g2", A64MCExpr::VK_ABS_G2)
                  .Case("abs_g2_s", A64MCExpr::VK_ABS_G2_S)
                  .Case("abs_g2_nc", A64MCExpr::VK_ABS_G2_NC)
                  .Case("abs_g1", A64MCExpr::VK_ABS_G1)
                  .Case("abs_g1_s", A64MCExpr::VK_ABS_G1_S)
                  .Case("abs_g1_nc", A64MCExpr::VK_ABS_G1_NC)
                  .Case("abs_g0", A64MCExpr::VK_ABS_G0)
                  .Case("abs_g0_s", A64MCExpr::VK_ABS_G0_S)
                  .Case("abs_g0_nc", A64MCExpr::VK_ABS_G0_NC)
                  .Default(A64MCExpr::VK_INVALID);

    if (RefKind == A64MCExpr::VK_INVALID)
      return TokError("expect relocation specifier in operand ':'");

    Parser.Lex(); // Eat identifier

    if (parseToken(AsmToken::Colon, "expect ':' after relocation specifier"))
      return true;
  }

  if (getParser().parseExpression(ImmVal))
    return true;

  if (HasELFModifier)
    ImmVal = A64MCExpr::create(ImmVal, RefKind, getContext());

  return false;
}

/// tryParseAdrLabel - Parse and validate a source label for the ADR
/// instruction.
OperandMatchResultTy A64AsmParser::tryParseAdrLabel(OperandVector &Operands) {
  SMLoc S = getLoc();
  const MCExpr *Expr = nullptr;

  // Leave anything with a bracket to the default for SVE
  if (getParser().getTok().is(AsmToken::LBrac))
    return MatchOperand_NoMatch;

  if (getParser().getTok().is(AsmToken::Hash))
    getParser().Lex(); // Eat hash token.

  if (parseSymbolicImmVal(Expr))
    return MatchOperand_ParseFail;

  A64MCExpr::VariantKind ELFRefKind;
  MCSymbolRefExpr::VariantKind DarwinRefKind;
  int64_t Addend;
  if (classifySymbolRef(Expr, ELFRefKind, DarwinRefKind, Addend)) {
    if (DarwinRefKind == MCSymbolRefExpr::VK_None &&
        ELFRefKind == A64MCExpr::VK_INVALID) {
      // No modifier was specified at all; this is the syntax for an ELF basic
      // ADR relocation (unfortunately).
      Expr = A64MCExpr::create(Expr, A64MCExpr::VK_ABS, getContext());
    } else {
      Error(S, "unexpected adr label");
      return MatchOperand_ParseFail;
    }
  }

  SMLoc E = SMLoc::getFromPointer(getLoc().getPointer() - 1);
  Operands.push_back(A64Operand::CreateImm(Expr, S, E, getContext()));
  return MatchOperand_Success;
}

/// tryParseAdrpLabel - Parse and validate a source label for the ADRP
/// instruction.
OperandMatchResultTy A64AsmParser::tryParseAdrpLabel(OperandVector &Operands) {
  MCAsmParser &Parser = getParser();
  SMLoc S = getLoc();
  const MCExpr *Expr = nullptr;

  if (Parser.getTok().is(AsmToken::Hash)) {
    Parser.Lex(); // Eat hash token.
  }

  if (parseSymbolicImmVal(Expr))
    return MatchOperand_ParseFail;

  A64MCExpr::VariantKind ELFRefKind;
  MCSymbolRefExpr::VariantKind DarwinRefKind;
  int64_t Addend;
  if (classifySymbolRef(Expr, ELFRefKind, DarwinRefKind, Addend)) {
    if (DarwinRefKind == MCSymbolRefExpr::VK_None &&
        ELFRefKind == A64MCExpr::VK_INVALID) {
      // No modifier was specified at all; this is the syntax for an ELF basic
      // ADRP relocation (unfortunately).
      Expr = A64MCExpr::create(Expr, A64MCExpr::VK_ABS_PAGE, getContext());
    } else {
      Error(S, "modifiers not supported yet");
      return MatchOperand_ParseFail;
    }
  }

  // We have either a label reference possibly with addend or an immediate. The
  // addend is a raw value here. The linker will adjust it to only reference the
  // page.
  SMLoc E = SMLoc::getFromPointer(getLoc().getPointer() - 1);
  Operands.push_back(A64Operand::CreateImm(Expr, S, E, getContext()));

  return MatchOperand_Success;
}

bool A64AsmParser::classifySymbolRef(
    const MCExpr *Expr, A64MCExpr::VariantKind &ELFRefKind,
    MCSymbolRefExpr::VariantKind &DarwinRefKind, int64_t &Addend) {
  ELFRefKind = A64MCExpr::VK_INVALID;
  DarwinRefKind = MCSymbolRefExpr::VK_None;
  Addend = 0;

  if (const A64MCExpr *AE = dyn_cast<A64MCExpr>(Expr)) {
    ELFRefKind = AE->getKind();
    Expr = AE->getSubExpr();
  }

  const MCSymbolRefExpr *SE = dyn_cast<MCSymbolRefExpr>(Expr);
  if (SE) {
    // It's a simple symbol reference with no addend.
    DarwinRefKind = SE->getKind();
    return true;
  }

  // Check that it looks like a symbol + an addend
  MCValue Res;
  bool Relocatable = Expr->evaluateAsRelocatable(Res, nullptr, nullptr);
  if (!Relocatable || Res.getSymB())
    return false;

  // Treat expressions with an ELFRefKind (like ":abs_g1:3", or
  // ":abs_g1:x" where x is constant) as symbolic even if there is no symbol.
  if (!Res.getSymA() && ELFRefKind == A64MCExpr::VK_INVALID)
    return false;

  if (Res.getSymA())
    DarwinRefKind = Res.getSymA()->getKind();
  Addend = Res.getConstant();

  // It's some symbol reference + a constant addend, but really
  // shouldn't use both Darwin and ELF syntax.
  return ELFRefKind == A64MCExpr::VK_INVALID ||
         DarwinRefKind == MCSymbolRefExpr::VK_None;
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
        return Error(IDLoc, "A64 too few operands for instruction",
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
