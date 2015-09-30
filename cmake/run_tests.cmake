step("Run try programm")
########################

find_program(TRY try
  PATHS ${CTEST_BINARY_DIRECTORY}
  PATH_SUFFIXES Debug Release RelWithDebInfo
  NO_DEFAULT_PATH
)
#message("try executable: ${TRY}")

execute_process(COMMAND ${TRY})
