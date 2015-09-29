ADD_LIBRARY(connector STATIC session.cc result.cc crud.cc document.cc)
ADD_CDK(connector)
target_compile_features(connector PUBLIC cxx_rvalue_references)
