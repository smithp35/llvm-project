include_directories( ${CMAKE_CURRENT_BINARY_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/.. )

add_llvm_component_library(LLVMA64Disassembler
  A64Disassembler.cpp
  A64ExternalSymbolizer.cpp

  LINK_COMPONENTS
  A64Desc
  A64Info
  MC
  MCDisassembler
  Support

  ADD_TO_COMPONENT
  A64
  )

add_dependencies(LLVMA64Disassembler A64CommonTableGen)
