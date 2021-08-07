//===-- A64TargetMachine.cpp - Define TargetMachine for A64 ---------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//
#include "A64TargetMachine.h"
#include "A64.h"
#include "A64TargetObjectFile.h"
#include "MCTargetDesc/A64MCTargetDesc.h"
#include "TargetInfo/A64TargetInfo.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/Function.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/CodeGen.h"

using namespace llvm;

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeA64Target() {
  // Register the target.
  RegisterTargetMachine<A64TargetMachine> X(getTheA64Target());
}

//===----------------------------------------------------------------------===//
// A64 Lowering public interface.
//===----------------------------------------------------------------------===//
static std::unique_ptr<TargetLoweringObjectFile> createTLOF(const Triple &TT) {
  return std::make_unique<A64_ELFTargetObjectFile>();
}

// Helper function to build a DataLayout string
static std::string computeDataLayout(const Triple &TT,
                                     const MCTargetOptions &Options) {
  std::string Ret = "";

  // Little endian
  Ret += "e";

  // ELF name mangling
  Ret += "-m:e";

  // 64-bit pointers, 64-bit aligned
  Ret += "-p:64:64";

  // 64-bit integers, 64 bit aligned
  Ret += "-i64:64";

  // 64-bit native integer width i.e register are 32-bit
  Ret += "-n64";

  // 128-bit natural stack alignment
  Ret += "-S128";

  return Ret;
}

static Reloc::Model getEffectiveRelocModel(const Triple &TT,
                                           Optional<Reloc::Model> RM) {
  if (!RM.has_value() || *RM == Reloc::DynamicNoPIC)
    return Reloc::Static;
  return *RM;
}

static CodeModel::Model getEffectiveA64CodeModel(const Triple &TT,
                                                 Optional<CodeModel::Model> CM,
                                                 bool JIT) {
  return CodeModel::Small;
}

/// Create an A64 architecture model.
///
A64TargetMachine::A64TargetMachine(const Target &T, const Triple &TT,
                                   StringRef CPU, StringRef FS,
                                   const TargetOptions &Options,
                                   Optional<Reloc::Model> RM,
                                   Optional<CodeModel::Model> CM,
                                   CodeGenOpt::Level OL, bool JIT)
    : LLVMTargetMachine(T, computeDataLayout(TT, Options.MCOptions), TT, CPU,
                        FS, Options, getEffectiveRelocModel(TT, RM),
                        getEffectiveA64CodeModel(TT, CM, JIT), OL),
      TLOF(createTLOF(getTargetTriple())) {
  initAsmInfo();
}

const A64Subtarget *
A64TargetMachine::getSubtargetImpl(const Function &F) const {
  Attribute CPUAttr = F.getFnAttribute("target-cpu");
  Attribute FSAttr = F.getFnAttribute("target-features");

  std::string CPU = !CPUAttr.hasAttribute(Attribute::None)
                        ? CPUAttr.getValueAsString().str()
                        : TargetCPU;
  std::string FS = !FSAttr.hasAttribute(Attribute::None)
                       ? FSAttr.getValueAsString().str()
                       : TargetFS;

  auto &I = SubtargetMap[CPU + FS];
  if (!I) {
    // This needs to be done before we create a new subtarget since any
    // creation will depend on the TM and the code generation flags on the
    // function that reside in TargetOptions.
    resetTargetOptions(F);
    I = std::make_unique<A64Subtarget>(TargetTriple, CPU, FS, *this);
  }
  return I.get();
}

namespace {

/// A64 Code Generator Pass Configuration Options.
class A64PassConfig : public TargetPassConfig {
public:
  A64PassConfig(A64TargetMachine &TM, PassManagerBase &PM)
      : TargetPassConfig(TM, PM) {}

  A64TargetMachine &getA64TargetMachine() const {
    return getTM<A64TargetMachine>();
  }

  bool addInstSelector() override {
    addPass(createA64ISelDag(getA64TargetMachine(), getOptLevel()));
    return false;
  }
};
} // end anonymous namespace

TargetPassConfig *
A64TargetMachine::createPassConfig(PassManagerBase &PM) {
  return new A64PassConfig(*this, PM);
}
