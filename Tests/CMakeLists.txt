SET(TEST_SOURCES
	main.cpp

	math.cpp
	resources.cpp
	scripts.cpp	
)

ADD_EXECUTABLE(Testsuite ${TEST_SOURCES})

TARGET_LINK_LIBRARIES(Testsuite
	LD31
	nowide-static
	
	${Angelscript_LIBRARIES}
	${SFML_LIBRARIES}
)

#SET_TARGET_PROPERTIES(Testsuite PROPERTIES
#	RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/Tests/TestTexture.png TestTexture.png COPYONLY)
