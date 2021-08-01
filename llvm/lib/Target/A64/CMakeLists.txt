add_llvm_component_group(A64)

set(LLVM_TARGET_DEFINITIONS A64.td)

tablegen(LLVM A64GenAsmMatcher.inc -gen-asm-matcher)
tablegen(LLVM A64GenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM A64GenCallingConv.inc -gen-callingconv)
tablegen(LLVM A64GenDAGISel.inc -gen-dag-isel)
tablegen(LLVM A64GenDisassemblerTables.inc -gen-disassembler)
tablegen(LLVM A64GenInstrInfo.inc -gen-instr-info)
tablegen(LLVM A64GenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM A64GenRegisterBank.inc -gen-register-bank)
tablegen(LLVM A64GenRegisterInfo.inc -gen-register-info)
tablegen(LLVM A64GenSubtargetInfo.inc -gen-subtarget)

add_public_tablegen_target(A64CommonTableGen)

add_llvm_target(A64CodeGen
  A64AsmPrinter.cpp
  A64FrameLowering.cpp
  A64InstrInfo.cpp
  A64ISelDAGToDAG.cpp
  A64ISelLowering.cpp
  A64MCInstLower.cpp
  A64RegisterInfo.cpp
  A64Subtarget.cpp
  A64TargetObjectFile.cpp
  A64TargetMachine.cpp

  LINK_COMPONENTS
  A64Desc
  A64Info
  Analysis
  AsmPrinter
  CodeGen
  Core
  MC
  Scalar
  SelectionDAG
  Support
  Target
  TransformUtils
  GlobalISel
  CFGuard

  ADD_TO_COMPONENT
  A64
)

add_subdirectory(AsmParser)
add_subdirectory(Disassembler)
add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
