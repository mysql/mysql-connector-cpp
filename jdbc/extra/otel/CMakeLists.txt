#
# Copyright (c) 2022, 2023, Oracle and/or its affiliates.
#
project(otel)

add_subdirectory(opentelemetry-cpp-1.8.3/api)

# Export targets

export(
  TARGETS 
# Note: opentelemetry_api is header-only -- see the generated exports.cmake
  opentelemetry_api
# TODO: Are these really needed for our purposes? It seems these libraires 
# depend on opentelemetry_sdk and we should not need to link to SDK
#    opentelemetry_common
#    opentelemetry_resources
#    opentelemetry_trace
  NAMESPACE otel_
  FILE "${PROJECT_BINARY_DIR}/exports.cmake"
)
