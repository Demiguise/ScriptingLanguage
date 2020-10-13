set(SRCS
  executor.h
  executor.cpp

  tokeniser.h
  tokeniser.cpp

  ASTree.h
  ASTree.cpp

  stack.h
  stack.cpp

  types.h
  types.cpp

  variables.h
  variables.cpp

  errors.h
  errors.cpp
)

set(Common_Defines
)

set(Debug_Defines
  _DEBUG
)

set(Common_Options
)

set(Debug_Options
)

set(Libs
)

#Test executable (To be removed once we're closer to finishing)
add_executable(TEST_HARNESS
  ${SRCS}
  win32_harness.cpp
)

set_target_properties(TEST_HARNESS PROPERTIES
  OUTPUT_NAME "test_harness"
  RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
)

target_include_directories(TEST_HARNESS
  PUBLIC
    ${PROJECT_SOURCE_DIR}/include
  PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)

target_compile_definitions(TEST_HARNESS
  PRIVATE
    ${Common_Defines}
    $<$<CONFIG:DEBUG>:${Debug_Defines}>
)

target_compile_options(TEST_HARNESS
  PRIVATE
    ${Common_Options}
    $<$<CONFIG:DEBUG>:${Debug_Options}>
)
