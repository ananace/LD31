# The sources

CONFIGURE_FILE(Defines.hpp.in ${CURRENT_SOURCE_DIR}/Defines.hpp)

SET(LD31_HEADERS
	Script/ScriptExtensions.hpp
	Script/ScriptManager.hpp
	Script/ScriptObject.hpp
	Script/SFML/Extensions.hpp

	Util/RefCount.hpp
    Util/RefCount.inl
	Util/ResourceManager.hpp
	Util/ResourceManager.inl
	Util/Time.hpp

    Math/Common.hpp
    Math/Common.inl
    Math/Rect.hpp
    Math/Rect.inl
    Math/Spinor.hpp
    Math/Vector.hpp
    Math/Vector.inl
)

SET(LD31_SOURCES
	Script/ScriptExtensions.cpp
	Script/ScriptManager.cpp
	Script/ScriptObject.cpp

	Script/SFML/CircleShape.cpp
	Script/SFML/Color.cpp
	Script/SFML/ConvexShape.cpp
	Script/SFML/Font.cpp
	Script/SFML/Music.cpp
	Script/SFML/RectangleShape.cpp
	Script/SFML/RenderTarget.cpp
	Script/SFML/Shader.cpp
	Script/SFML/Sound.cpp
	Script/SFML/Sprite.cpp
	Script/SFML/Text.cpp
	Script/SFML/Texture.cpp
	Script/SFML/View.cpp

    Util/RefCount.cpp
    Util/Time.cpp

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

SOURCE_GROUP("Header files\\Script\\SFML" REGULAR_EXPRESSION "Script/SFML/[^./]+.(hpp|inl)")
SOURCE_GROUP("Source files\\Script\\SFML" REGULAR_EXPRESSION "Script/SFML/[^./]+.cpp")

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
	nowide-static
	
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