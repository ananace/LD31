CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

PROJECT(LD31 VERSION 0.0.0.1)

# Pretests

IF (MSVC)
	IF (${MSVC_VERSION} VERSION_LESS 11.0)
		MESSAGE(FATAL_ERROR "This project uses C++11, so you need at least Visual Studio 2012")
	ENDIF ()

	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /MP")
	SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL")
	SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
ELSE () # Linux / Cygwin
	IF (${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
		EXECUTE_PROCESS(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
		IF (${GCC_VERSION} VERSION_LESS 4.7)
			MESSAGE(FATAL_ERROR "This project uses C++11, so you need at least GCC 4.7 to compile this project.")
		ENDIF ()
	ELSEIF (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
		EXECUTE_PROCESS( COMMAND ${CMAKE_CXX_COMPILER} --version OUTPUT_VARIABLE clang_full_version_string )
		STRING (REGEX REPLACE ".*clang version ([0-9]+\\.[0-9]+).*" "\\1" CLANG_VERSION_STRING ${clang_full_version_string})

		IF (${CLANG_VERSION_STRING} VERSION_LESS 3.3)
			MESSAGE(FATAL_ERROR "This project uses C++11, so you need at least Clang 3.3 to compile this project.")
		ENDIF ()

		ADD_DEFINITIONS(-DAS_CAN_USE_CPP11 -D__GNUC_MINOR__=8) # Clang hack, makes asGetTypeTraits work
	ENDIF ()

	SET(CMAKE_CXX_FLAGS                "-Wall -Wextra -std=c++11 -pedantic")
	SET(CMAKE_CXX_FLAGS_DEBUG          "-O0 -g")
	SET(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
	SET(CMAKE_CXX_FLAGS_RELEASE        "-O2 -DNDEBUG")
	SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
ENDIF ()

# Options

OPTION(LD31_TESTS "Build the test cases?")
OPTION(LD31_JIT "Use the Angelscript JIT compiler?")

# CMake modules

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)

IF (EXISTS ${PROJECT_SOURCE_DIR}/.git)
	FIND_PACKAGE(Git)
	IF (GIT_FOUND)
		INCLUDE("${CMAKE_MODULE_PATH}/GetGitRevisionDescription.cmake")

		GIT_DESCRIBE(LD31_GIT_DESCRIBE --long)
		IF (LD31_GIT_DESCRIBE)
			SET(LD31_HAS_TAG 1)
		ELSE()
			SET(LD31_HAS_TAG 0)
		ENDIF()
	ENDIF()
ENDIF()

# Find packages

IF (LD31_JIT)
	FIND_PACKAGE(AngelscriptJIT REQUIRED)
ENDIF()

FIND_PACKAGE(Angelscript COMPONENTS scriptarray scriptbuilder scriptdictionary scriptfile scriptgrid scripthelper scriptmath scriptstdstring REQUIRED)
IF (MSVC)
	FIND_PACKAGE(SFML 2 COMPONENTS main system window graphics audio REQUIRED)
ELSE()
	FIND_PACKAGE(SFML 2 COMPONENTS system window graphics audio REQUIRED)
ENDIF()

IF (MSVC)
	ADD_SUBDIRECTORY(External/nowide_standalone)
	SET(Nowide_LIBRARIES nowide-static)
ELSEIF (APPLE)
	# Apple specific CMake code
ELSE()
	FIND_PATH(FontConfig_INCLUDE_DIR fontconfig/fontconfig.h)
	FIND_LIBRARY(FontConfig_LIBRARIES NAMES fontconfig)

	INCLUDE_DIRECTORIES(${FontConfig_INCLUDE_DIR})
ENDIF()

# Include directories

INCLUDE_DIRECTORIES(
	Source
	External/
	External/nowide_standalone

	${Angelscript_INCLUDE_DIR}
	${Angelscript_ADDON_DIR}
	${SFML_INCLUDE_DIR}
)

# Main Source

ADD_SUBDIRECTORY(Source)

# Tests

IF (LD31_TESTS)
	ADD_SUBDIRECTORY(Tests/Catch)
	INCLUDE_DIRECTORIES(
		${CATCH_INCLUDE_DIR}
	)

	ENABLE_TESTING()

	ADD_SUBDIRECTORY(Tests)

	ADD_TEST(NAME LD32_Testsuite
			 COMMAND TestSuite
			 WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/Tests)
ENDIF()

# Copy scripts

IF (WIN32 AND MSVC)
	FOREACH(build Debug Release RelWithDebInfo)
		IF (IS_DIRECTORY ${PROJECT_BINARY_DIR}/bin/${build})
			FILE(COPY "Scripts" DESTINATION ${PROJECT_BINARY_DIR}/bin/${build})
		ENDIF()
	ENDFOREACH()
ELSE()
	FILE(COPY "Scripts" DESTINATION ${PROJECT_BINARY_DIR}/bin)
ENDIF()
