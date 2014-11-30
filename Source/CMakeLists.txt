# The sources

CONFIGURE_FILE(Defines.hpp.in ${CURRENT_SOURCE_DIR}/Defines.hpp)

SET(LD31_HEADERS
	Script/ScriptEngine.hpp
	Script/ScriptExtensions.hpp
	Script/ScriptManager.hpp

	Util/RefCount.hpp
    Util/RefCount.inl
	Util/ResourceManager.hpp
	Util/ResourceManager.inl

    Math/Common.hpp
    Math/Common.inl
    Math/Rect.hpp
    Math/Rect.inl
    Math/Spinor.hpp
    Math/Vector.hpp
    Math/Vector.inl
)

SET(LD31_SOURCES
	Script/ScriptEngine.cpp
	Script/ScriptExtensions.cpp
	Script/ScriptManager.cpp

    Util/RefCount.cpp

    Math/Common.cpp
    Math/Rect.cpp
    Math/Spinor.cpp
    Math/Vector.cpp
)

SET(MISC_HEADERS
	${Angelscript_HEADER}
)
SET(MISC_SOURCES
	${Angelscript_SOURCE}
)

# Source groups

SOURCE_GROUP("Header files\\Script" REGULAR_EXPRESSION "Script/[^./]+.(hpp|inl)")
SOURCE_GROUP("Source files\\Script" REGULAR_EXPRESSION "Script/[^./]+.cpp")

SOURCE_GROUP("Header files\\Util" REGULAR_EXPRESSION "Util/[^./]+.(hpp|inl)")
SOURCE_GROUP("Source files\\Util" REGULAR_EXPRESSION "Util/[^./]+.cpp")

SOURCE_GROUP("Header files\\Math" REGULAR_EXPRESSION "Math/[^./]+.(hpp|inl)")
SOURCE_GROUP("Source files\\Math" REGULAR_EXPRESSION "Math/[^./]+.cpp")

SOURCE_GROUP("Header files\\Misc\\Angelscript" FILES ${Angelscript_HEADER})
SOURCE_GROUP("Source files\\Misc\\Angelscript" FILES ${Angelscript_SOURCE})

IF (LD31_JIT)
	SOURCE_GROUP("Source files\\Misc\\AngelscriptJIT" FILES ${AngelscriptJIT_SOURCES})
ENDIF()

# The game

ADD_LIBRARY(LD31 STATIC
	${LD31_HEADERS}
	${LD31_SOURCES}

	${MISC_HEADERS}
	${MISC_SOURCES}
)

ADD_EXECUTABLE(LD31_Game
	main.cpp
)

TARGET_LINK_LIBRARIES(LD31_Game
	LD31
	
	${Angelscript_LIBRARIES}
	${SFML_LIBRARIES}
)

SET_TARGET_PROPERTIES(LD31_Game PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

# Only show console on Debug builds

IF(WIN32)
   SET_TARGET_PROPERTIES(LD31_Game PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
   SET_TARGET_PROPERTIES(LD31_Game PROPERTIES COMPILE_DEFINITIONS_DEBUG "_CONSOLE")
   SET_TARGET_PROPERTIES(LD31_Game PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:CONSOLE")
   SET_TARGET_PROPERTIES(LD31_Game PROPERTIES COMPILE_DEFINITIONS_RELWITHDEBINFO "_CONSOLE")
   SET_TARGET_PROPERTIES(LD31_Game PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
   SET_TARGET_PROPERTIES(LD31_Game PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS")
ENDIF(WIN32)