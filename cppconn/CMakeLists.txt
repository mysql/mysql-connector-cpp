#   Copyright (C) 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.
#
#   The MySQL Connector/C++ is licensed under the terms of the GPL
#   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
#   MySQL Connectors. There are special exceptions to the terms and
#   conditions of the GPL as it is applied to this software, see the
#   FLOSS License Exception
#   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.

CHECK_INCLUDE_FILES(stdint.h HAVE_STDINT_H)
CHECK_INCLUDE_FILES(inttypes.h HAVE_INTTYPES_H)

CHECK_TYPE_SIZE("int8_t"   HAVE_INT8_T)
CHECK_TYPE_SIZE("uint8_t"  HAVE_UINT8_T)
CHECK_TYPE_SIZE("int16_t"  HAVE_INT16_T)
CHECK_TYPE_SIZE("uint16_t" HAVE_UINT16_T)
CHECK_TYPE_SIZE("int32_t"  HAVE_INT32_T)
CHECK_TYPE_SIZE("uint32_t" HAVE_UINT32_T)
CHECK_TYPE_SIZE("int64_t"  HAVE_INT64_T)
CHECK_TYPE_SIZE("uint64_t" HAVE_UINT64_T)
CHECK_TYPE_SIZE("__int8"			HAVE_MS_INT8)
CHECK_TYPE_SIZE("unsigned __int8" 	HAVE_MS_UINT8)
CHECK_TYPE_SIZE("__int16"			HAVE_MS_INT16)
CHECK_TYPE_SIZE("unsigned __int16"	HAVE_MS_UINT16)
CHECK_TYPE_SIZE("__int32"			HAVE_MS_INT32)
CHECK_TYPE_SIZE("unsigned __int32"	HAVE_MS_UINT32)
CHECK_TYPE_SIZE("__int64"			HAVE_MS_INT64)
CHECK_TYPE_SIZE("unsigned __int64"	HAVE_MS_UINT64)


CHECK_TYPE_SIZE("wchar_t" SIZEOF_WCHAR_T)
CHECK_TYPE_SIZE("char" SIZEOF_CHAR)
CHECK_TYPE_SIZE("short" SIZEOF_SHORT)
CHECK_TYPE_SIZE("int" SIZEOF_INT)
CHECK_TYPE_SIZE("long" SIZEOF_LONG)
CHECK_TYPE_SIZE("long long" SIZEOF_LONG_LONG)
CHECK_TYPE_SIZE("float" SIZEOF_FLOAT)
CHECK_TYPE_SIZE("double" SIZEOF_DOUBLE)
CHECK_TYPE_SIZE("long double" SIZEOF_LONG_DOUBLE)
CHECK_TYPE_SIZE("size_t" SIZEOF_SIZE_T)
CHECK_TYPE_SIZE("ssize_t" SIZEOF_SSIZE_T)
CHECK_TYPE_SIZE("off_t" SIZEOF_OFF_T)
CHECK_TYPE_SIZE("ptrdiff_t" SIZEOF_PTRDIFF_T)
CHECK_TYPE_SIZE("intptr_t" SIZEOF_INTPTR_T)
CHECK_TYPE_SIZE("uintptr_t" SIZEOF_UINTPTR_T)
CHECK_TYPE_SIZE("intmax_t" SIZEOF_INTMAX_T)
CHECK_TYPE_SIZE("uintmax_t" SIZEOF_UINTMAX_T)

CHECK_TYPE_SIZE("time_t" SIZEOF_TIME_T)
CHECK_TYPE_SIZE("clockid_t" SIZEOF_CLOCKID_T)
CHECK_TYPE_SIZE("timer_t" SIZEOF_TIMER_T)
CHECK_TYPE_SIZE("pid_t" SIZEOF_PID_T)
CHECK_TYPE_SIZE("gid_t" SIZEOF_GID_T)
CHECK_TYPE_SIZE("uid_t" SIZEOF_UID_T)


MESSAGE(STATUS "size of char is        : ${SIZEOF_CHAR}")
MESSAGE(STATUS "size of wchar_t is     : ${SIZEOF_WCHAR_T}")
MESSAGE(STATUS "size of short is       : ${SIZEOF_SHORT}")
MESSAGE(STATUS "size of int is         : ${SIZEOF_INT}")
MESSAGE(STATUS "size of long is        : ${SIZEOF_LONG}")
MESSAGE(STATUS "size of long long is   : ${SIZEOF_LONG_LONG}")
MESSAGE(STATUS "size of float is       : ${SIZEOF_FLOAT}")
MESSAGE(STATUS "size of double is      : ${SIZEOF_DOUBLE}")
MESSAGE(STATUS "size of long double is : ${SIZEOF_LONG_DOUBLE}")
MESSAGE(STATUS "size of size_t is      : ${SIZEOF_SIZE_T}")
MESSAGE(STATUS "size of ssize_t is     : ${SIZEOF_SSIZE_T}")
MESSAGE(STATUS "size of off_t is       : ${SIZEOF_OFF_T}")
MESSAGE(STATUS "size of prtdiff_t is   : ${SIZEOF_PTRDIFF_T}")
MESSAGE(STATUS "size of intptr_t is    : ${SIZEOF_INTPTR_T}")
MESSAGE(STATUS "size of uintptr_t is   : ${SIZEOF_UINTPTR_T}")
MESSAGE(STATUS "size of intmax_t is    : ${SIZEOF_INTMAX_T}")
MESSAGE(STATUS "size of uintmax_t is   : ${SIZEOF_UINTMAX_T}")
MESSAGE(STATUS "size of time_t is      : ${SIZEOF_TIME_T}")
MESSAGE(STATUS "size of clock_id is    : ${SIZEOF_CLOCKID_T}")
MESSAGE(STATUS "size of timer_t is     : ${SIZEOF_TIMER_T}")
MESSAGE(STATUS "size of pid_t is       : ${SIZEOF_PID_T}")
MESSAGE(STATUS "size of gid_t is       : ${SIZEOF_GID_T}")
MESSAGE(STATUS "size of uid_t is       : ${SIZEOF_UID_T}")

CHECK_FUNCTION_EXISTS(strtold HAVE_FUNCTION_STRTOLD)
CHECK_FUNCTION_EXISTS(strtol HAVE_FUNCTION_STRTOL)
CHECK_FUNCTION_EXISTS(strtoll HAVE_FUNCTION_STRTOLL)
CHECK_FUNCTION_EXISTS(strtoul HAVE_FUNCTION_STRTOUL)
CHECK_FUNCTION_EXISTS(strtoull HAVE_FUNCTION_STRTOULL)
CHECK_FUNCTION_EXISTS(strtoimax HAVE_FUNCTION_STRTOIMAX)
CHECK_FUNCTION_EXISTS(strtoumax HAVE_FUNCTION_STRTOUMAX)


CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cm ${CMAKE_CURRENT_BINARY_DIR}/config.h)


SET(MYSQLCPPCONN_INSTALL_HEADERS
		config.h
		connection.h
		datatype.h
		driver.h
		exception.h
		metadata.h
		parameter_metadata.h
		prepared_statement.h
		resultset.h
		resultset_metadata.h
		statement.h
		warning.h)

INSTALL(FILES ${MYSQLCPPCONN_INSTALL_HEADERS} DESTINATION include/cppconn)
