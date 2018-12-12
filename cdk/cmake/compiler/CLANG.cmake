
function(enable_cxx11)

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

  add_flags(CXX -std=c++11 -stdlib=libc++)

endfunction()


function(set_visibility)
  add_compile_options(-fvisibility-ms-compat)
endfunction()

