ADD_LIBRARY(concplus STATIC session.cc result.cc crud.cc)
ADD_CDK(concplus)
target_compile_features(concplus PUBLIC cxx_rvalue_references)
