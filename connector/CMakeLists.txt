#
# We use UUID generator from common sub-module.
#

set(WITH_UUID "${PROJECT_SOURCE_DIR}/cdk/common/uuid")

if(NOT EXISTS "${WITH_UUID}/src/CMakeLists.txt")
  message(FATAL_ERROR "Could not find UUID generator sources at: ${WITH_UUID}")
endif()


include_directories("${WITH_UUID}/include")

add_subdirectory("${WITH_UUID}/src" uuid)

ADD_LIBRARY(connector STATIC session.cc result.cc crud.cc document.cc)
ADD_CDK(connector)
target_link_libraries(connector uuid_gen)
target_compile_features(connector PUBLIC cxx_rvalue_references)
