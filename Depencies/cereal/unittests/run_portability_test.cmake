macro(EXEC_CMD_CHECK)
  message("running ${ARGN}")
  execute_process(COMMAND ${ARGN} RESULT_VARIABLE CMD_RESULT)
  if(CMD_RESULT)
    message(FATAL_ERROR "Error running ${ARGN}")
  endif()
endmacro()

set(PORTABILITY_TEST_64 "${PORTABILITY_TEST_DIR}/portability_test64")

exec_cmd_check(${PORTABILITY_TEST_64} save 64)
exec_cmd_check(${PORTABILITY_TEST_64} load 64)
