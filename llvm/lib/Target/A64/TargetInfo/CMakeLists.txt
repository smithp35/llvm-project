include_directories( ${CMAKE_CURRENT_BINARY_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/.. )

add_llvm_component_library(LLVMA64Info
  A64TargetInfo.cpp

  LINK_COMPONENTS
  Support

  ADD_TO_COMPONENT
  A64
  )

add_dependencies(LLVMA64Info A64CommonTableGen)
