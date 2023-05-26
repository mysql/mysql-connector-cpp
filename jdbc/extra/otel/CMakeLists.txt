#
# Copyright (c) 2022, 2023, Oracle and/or its affiliates.
#
project(otel)

add_subdirectory(opentelemetry-cpp-1.8.3/api)

# Export targets
# Note: opentelemetry_api is header-only library

export(
  TARGETS opentelemetry_api
  NAMESPACE otel_
  FILE "${PROJECT_BINARY_DIR}/exports.cmake"
)
