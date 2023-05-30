# Copyright (c) 2022, 2023, Oracle and/or its affiliates.

HOWTO import a new opentelemetry-cpp package 

Using release 1.6.0 as an example, adjust to the proper release number.

1) Read the release notes

https://github.com/open-telemetry/opentelemetry-cpp/releases/tag/v1.6.0

If opentelemetry-cpp upgraded its own dependencies,
the following packages may need to be upgraded as well:

- internal/extra/json
- internal/extra/opentelemetry-proto

2) Download the .tar.gz 

https://github.com/open-telemetry/opentelemetry-cpp/archive/refs/tags/v1.6.0.tar.gz

3) Unpack the .tar.gz in a sub directory

cd internal/extra/opentelemetry-cpp
tar xvf opentelemetry-cpp-1.6.0.tar.gz

This should create a directory named internal/extra/opentelemetry-cpp/opentelemetry-cpp-1.6.0

Code for a package MUST be in a dedicated sub directory.

4) Remove un necessary code

cd internal/extra/opentelemetry-cpp/opentelemetry-cpp-1.6.0

5) Commit and push.

It is important to have a separate commit for the import alone,
for the git history.

At this point, the new code is imported, but not used yet.

6) Adjust CMake

In internal/cmake/opentelemetry-cpp.cmake,
point the tag to the new release

SET(OPENTELEMETRY_CPP_TAG "opentelemetry-cpp-1.6.0")

7) Do a full build and test

In particular, make sure the code still builds in MYSQL_MAINTAINER_MODE,
and adjust warnings flags for third party code if needed.

See internal/extra/opentelemetry-cpp/CMakeFiles.txt

8) Commit and push.

At this point, the new code is used,
and the old code is still in the repository.

9) Cleanup the previous release

cd internal/extra/opentelemetry-cpp/
rm -rf opentelemetry-cpp-<old version>

10) Commit and push.

It is important to have a separate commit for the cleanup alone,
for the git history.

