file(GLOB TESTS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

# Build all of the non-special tests
foreach(TEST_SOURCE ${TESTS})
  message(STATUS ${TEST_SOURCE})

  string(REPLACE ".cpp" "" TEST_TARGET "${TEST_SOURCE}")
  set(TEST_TARGET "test_cpp17_${TEST_TARGET}")

  add_executable(${TEST_TARGET} ${TEST_SOURCE})：
  target_link_libraries(${TEST_TARGET} ${CEREAL_THREAD_LIBS})
  add_test(NAME "${TEST_TARGET}" COMMAND "${TEST_TARGET}")

  # If we are on a 64-bit machine, create an extra 64-bit version of the test if portability testing is enabled
  if((NOT MSVC) AND (${CMAKE_SIZEOF_VOID_P} EQUAL 8) AND (NOT SKIP_PORTABILITY_TEST))
    add_executable(${TEST_TARGET}_64 ${TEST_SOURCE})
    target_link_libraries(${TEST_TARGET}_64 ${CEREAL_THREAD_LIBS})
    set_target_properties(${TEST_TARGET}_64 PROPERTIES
      COMPILE_FLAGS "-m64" LINK_FLAGS "-m64")
    add_test(NAME "${TEST_TARGET}_64" COMMAND "${TEST_TARGET}_64")
  endif()

endforeach()

if(NOT MSVC)
  # add tests to coverage
  foreach(TEST_SOURCE ${TESTS})
    string(REPLACE ".cpp" "" COVERAGE_TARGET "${TEST_SOURCE}")
    set(COVERAGE_TARGET "coverage_cpp17_${COVERAGE_TARGET}")

    add_dependencies(coverage ${COVERAGE_TARGET})

    add_executable(${COVERAGE_TARGET} EXCLUDE_FROM_ALL ${TEST_SOURCE})
    set_target_properties(${COVERAGE_TARGET} PROPERTIES COMPILE_FLAGS "-coverage")
    set_target_properties(${COVERAGE_TARGET} PROPERTIES LINK_FLAGS "-coverage")
    set_target_properties(${COVERAGE_TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/coverage")
    target_link_libraries(${COVERAGE_TARGET} ${CEREAL_THREAD_LIBS})
  endforeach()
endif()
