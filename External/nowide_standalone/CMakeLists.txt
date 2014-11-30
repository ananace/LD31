cmake_minimum_required(VERSION 2.6)

include_directories(.)
set(CXX_FLAGS "/EHsc /W3")

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	set(NOWIDE_SUFFIX "-d")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_FLAGS}")

add_library(nowide-static STATIC src/iostream.cpp)
set_target_properties(nowide-static PROPERTIES
				CLEAN_DIRECT_OUTPUT 1
				OUTPUT_NAME "nowide${NOWIDE_SUFFIX}"
			)

set_target_properties(nowide-static PROPERTIES PREFIX "lib")
