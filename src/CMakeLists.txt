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

  common.cpp
  common.h
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

option(USE_UNIT_TESTS "Whether to use Catch2 Tests" On)

if (USE_UNIT_TESTS)
  message(STATUS "Building for unit tests")
  list(APPEND Common_Defines
    USE_UNIT_TESTS
  )
  list(APPEND SRCS 
    catch2_harness.cpp
  )
else()
  message(STATUS "Building for manual harness")
  list(APPEND SRCS 
    win32_harness.cpp
  )
endif()

#Test executable (To be removed once we're closer to finishing)
add_executable(TESTS
  ${SRCS}
)

set_target_properties(TESTS PROPERTIES
  OUTPUT_NAME "test_harness"
  RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
)

target_include_directories(TESTS
  PUBLIC
    ${PROJECT_SOURCE_DIR}/include
  PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)

target_compile_definitions(TESTS
  PRIVATE
    ${Common_Defines}
    $<$<CONFIG:DEBUG>:${Debug_Defines}>
)

target_compile_options(TESTS
  PRIVATE
    ${Common_Options}
    $<$<CONFIG:DEBUG>:${Debug_Options}>
)

if (USE_UNIT_TESTS)
  target_link_libraries(TESTS
    PRIVATE
      Catch2::Catch2
  )
endif()
