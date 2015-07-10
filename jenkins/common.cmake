
macro(check_param name msg)
if (NOT DEFINED ${name})
  set(${name} $ENV{${name}})
endif()
if (NOT DEFINED ${name})
  message(FATAL_ERROR "Set ${msg} using ${name} variable")
endif()
endmacro()


macro(set_param name doc)
if (NOT DEFINED ${name})
  set(${name} $ENV{${name}})
endif()
if (NOT DEFINED ${name} AND ${ARGC} GREATER 2)
  set(${name} ${ARGN})
endif()
endmacro()


macro(start)
message("\n==== START ====\n")
ctest_start("Experimental")
message("\n===============\n")
endmacro()

macro(step msg)
message("\n---- ${msg}\n")
endmacro()

macro(fail msg)
message(FATAL_ERROR ${msg})
endmacro()

macro(done)
message("\n==== DONE ====\n")
endmacro()
