include_directories( ${CMAKE_CURRENT_BINARY_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/.. )

add_llvm_component_library(LLVMA64AsmParser
  A64AsmParser.cpp

  LINK_COMPONENTS
  A64Desc
  A64Info
  MC
  MCParser
  Support

  ADD_TO_COMPONENT
  A64
  )
