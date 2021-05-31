add_llvm_component_library(LLVMA64Desc
  A64ELFStreamer.cpp
  A64ELFObjectWriter.cpp
  A64MCAsmInfo.cpp
  A64MCAsmBackend.cpp
  A64MCCodeEmitter.cpp
  A64MCExpr.cpp
  A64InstPrinter.cpp
  A64MCTargetDesc.cpp
  A64TargetStreamer.cpp

  LINK_COMPONENTS
  A64Info
  MC
  BinaryFormat
  Support

  ADD_TO_COMPONENT
  A64
)

add_dependencies(LLVMA64Desc A64CommonTableGen)

# Hack: we need to include 'main' target directory to grab private headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_BINARY_DIR}/..)
