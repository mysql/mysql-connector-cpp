# NOTE: target_compile_features() is available in CMake 3.1.3

function(target_compile_features TARGET MODE)

if (CMAKE_COMPILER_IS_GNUCXX)
  target_compile_options(${TARGET} ${MODE} "-std=c++11")
  return()
endif()

endfunction()


function(set_interface_options TARGET DEP)

get_property(opts TARGET ${DEP} PROPERTY INTERFACE_COMPILE_OPTIONS)
#message("setting interface options for dependency ${DEP} on ${TARGET} to: ${opts}")
target_compile_options(${TARGET} PRIVATE ${opts})

endfunction()
