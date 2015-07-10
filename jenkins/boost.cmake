#
# Download protobuf dependency
#
# -DPROTBUF_URL=<url>
#

if(NOT BOOST_URL)
  message(FATAL_ERROR "Specify URL to load from with BOOST_URL")
endif()

if(NOT EXISTS boost-1.56)

if(NOT EXISTS boost.tar.gz)

  message("Downloading boost from: ${BOOST_URL}")

  file(DOWNLOAD  ${BOOST_URL} ./boost.tar.gz TIMEOUT 600 STATUS status)
  message("status: ${status}")
  list(GET status 0 status_code)

  if(status_code)
    message(FATAL_ERROR "Failed to download")
  endif()

endif(NOT EXISTS boost.tar.gz)

message("uncompressing")
execute_process(COMMAND tar xzf boost.tar.gz RESULT_VARIABLE uncompress_result)
message("uncompress_result: ${uncompress_result}")

if(uncompress_result)
  message(FATAL_ERROR "Could not uncompress: ${uncompress_result}")
endif()

# rename protobuf directory

file(GLOB boost_dir "boost_1_56_*")

if(NOT boost_dir)
  message(FATAL_ERROR "Could not find boost 1.56 folder after decompression")
endif()

file(RENAME ${boost_dir} "boost-1.56")

endif(NOT EXISTS boost-1.56)

