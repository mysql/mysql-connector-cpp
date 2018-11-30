#
# If Clang is used and deployment target is not specified
# with MACOSX_DEPLOYMENT_TARGET environment variable, make
# sure that clang's native implementation of C++ std
# libarary (libc++) is used. Otherwise clang defaults to
# GNU version (libstdc++) which is outdated and does
# not handle C++11 well.
#
# TODO: Add option to use the default runtime if user wishes
# so.
#

function(set_visibility)
  add_compile_options(-fvisibility-ms-compat)
endfunction()

# ???

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang"
   AND NOT DEFINED ENV{MACOSX_DEPLOYMENT_TARGET}
   AND NOT CMAKE_SYSTEM_NAME MATCHES "Linux")

  list(APPEND CPP11FLAGS -stdlib=libc++)

endif()
