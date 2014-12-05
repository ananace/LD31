# The sources

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Defines.hpp.in ${CMAKE_CURRENT_SOURCE_DIR}/Defines.hpp)

SET(LD31_HEADERS
	Defines.hpp

	Input/Input.hpp
	Input/InputManager.hpp
	Input/InputManager.inl
	Input/JoystickCurve.hpp

	Math/Common.hpp
	Math/Common.inl
	Math/Rect.hpp
	Math/Rect.inl
	Math/Spinor.hpp
	Math/Vector.hpp
	Math/Vector.inl

	Script/ScriptExtensions.hpp
	Script/ScriptHooks.hpp
	Script/ScriptHooks.inl
	Script/ScriptInterface.hpp
	Script/ScriptManager.hpp
	Script/ScriptObject.hpp

	Script/SFML/Extensions.hpp
	Script/SFML/Extensions.inl

	Util/DateTime.hpp
	Util/FileSystem.hpp
	Util/FontFinder.hpp
	Util/INIFile.hpp
	Util/RefCount.hpp
	Util/RefCount.inl
	Util/ResourceManager.hpp
	Util/ResourceManager.inl
	Util/Time.hpp
)

SET(LD31_SOURCES
	Input/Input.cpp
	Input/InputManager.cpp
	Input/JoystickCurve.cpp

	Math/Common.cpp
	Math/Rect.cpp
	Math/Spinor.cpp
	Math/Vector.cpp

	Script/ScriptExtensions.cpp
	Script/ScriptHooks.cpp
	Script/ScriptInterface.cpp
	Script/ScriptManager.cpp
	Script/ScriptObject.cpp

	Script/SFML/Extensions.cpp
	Script/SFML/CircleShape.cpp
	Script/SFML/Color.cpp
	Script/SFML/ConvexShape.cpp
	Script/SFML/Font.cpp
	Script/SFML/Mouse.cpp
	Script/SFML/Music.cpp
	Script/SFML/RectangleShape.cpp
	Script/SFML/RenderTarget.cpp
	Script/SFML/Shader.cpp
	Script/SFML/Sound.cpp
	Script/SFML/Sprite.cpp
	Script/SFML/Text.cpp
	Script/SFML/Texture.cpp
	Script/SFML/View.cpp

	Util/DateTime.cpp
	Util/FileSystem.cpp
	Util/INIFile.cpp
	Util/FontFinder.cpp
	Util/RefCount.cpp
	Util/Time.cpp
)

SET(MISC_HEADERS
	${Angelscript_HEADER}
)
SET(MISC_SOURCES
	${Angelscript_SOURCE}
)

# Source groups
SOURCE_GROUP("Header files\\Input" REGULAR_EXPRESSION "Input/[^./]+.(hpp|inl)")
SOURCE_GROUP("Source files\\Input" REGULAR_EXPRESSION "Input/[^./]+.cpp")

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

# The game "engine"

ADD_LIBRARY(LD31 STATIC
	${LD31_HEADERS}
	${LD31_SOURCES}

	${MISC_HEADERS}
	${MISC_SOURCES}
)

ADD_SUBDIRECTORY(Game)
