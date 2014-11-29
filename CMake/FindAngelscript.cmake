include(FindPackageHandleStandardArgs)

FIND_PATH(Angelscript_INCLUDE_DIR angelscript.h)
FIND_PATH(Angelscript_ADDON_DIR scriptstdstring/scriptstdstring.h)

SET(Angelscript_NAMES ${Angelscript_NAMES} angelscript)
FIND_LIBRARY(Angelscript_LIBRARY_DEBUG
  NAMES ${Angelscript_NAMES}d
  )

FIND_LIBRARY(Angelscript_LIBRARY_RELEASE
  NAMES ${Angelscript_NAMES}
  )

IF (Angelscript_LIBRARY_DEBUG AND Angelscript_LIBRARY_RELEASE)
   SET(Angelscript_LIBRARIES debug ${Angelscript_LIBRARY_DEBUG} optimized ${Angelscript_LIBRARY_RELEASE})
ELSEIF (Angelscript_LIBRARY_DEBUG AND NOT Angelscript_LIBRARY_RELEASE)
   SET(Angelscript_LIBRARIES ${Angelscript_LIBRARY_DEBUG})
ELSEIF (NOT Angelscript_LIBRARY_DEBUG AND Angelscript_LIBRARY_RELEASE)
   SET(Angelscript_LIBRARIES ${Angelscript_LIBRARY_RELEASE})
ELSE()
   SET(Angelscript_LIBRARIES "")
ENDIF()

find_package_handle_standard_args(Angelscript DEFAULT_MSG
                                  Angelscript_LIBRARIES Angelscript_INCLUDE_DIR)

IF(Angelscript_ADDON_DIR AND Angelscript_FIND_COMPONENTS)
  FOREACH(COMPONENT ${Angelscript_FIND_COMPONENTS})
    UNSET(OTHERH)
    UNSET(OTHERS)

    IF("${COMPONENT}" STREQUAL "scriptmath")
      SET(OTHERH "${COMPONENT}/scriptmathcomplex.h")
      SET(OTHERS "${COMPONENT}/scriptmathcomplex.cpp")
    ELSEIF("${COMPONENT}" STREQUAL "scriptstdstring")
      SET(OTHERS "${COMPONENT}/scriptstdstring_utils.cpp")
    ENDIF()

    FIND_FILE(Angelscript_${COMPONENT}_HEADER
        NAMES ${COMPONENT}/${COMPONENT}.h
        PATHS ${Angelscript_ADDON_DIR}
        )
    FIND_FILE(Angelscript_${COMPONENT}_SOURCE
        NAMES ${COMPONENT}/${COMPONENT}.cpp
        PATHS ${Angelscript_ADDON_DIR}
        )

    IF((OTHERH AND Angelscript_${COMPONENT}_HEADER) OR (OTHERS AND Angelscript_${COMPONENT}_SOURCE))
      IF(OTHERH)
        SET(Angelscript_${COMPONENT}_HEADER ${Angelscript_${COMPONENT}_HEADER} ${Angelscript_ADDON_DIR}/${OTHERH})
      ENDIF()

      IF(OTHERS)
        SET(Angelscript_${COMPONENT}_SOURCE ${Angelscript_${COMPONENT}_SOURCE} ${Angelscript_ADDON_DIR}/${OTHERS})
      ENDIF()
    ENDIF()

    IF(Angelscript_${COMPONENT}_HEADER AND Angelscript_${COMPONENT}_SOURCE)
        SET(Angelscript_${COMPONENT}_FOUND TRUE)
    ELSE()
        SET(Angelscript_${COMPONENT}_FOUND FALSE)
    ENDIF()

    SET(Angelscript_HEADER ${Angelscript_HEADER} ${Angelscript_${COMPONENT}_HEADER})
    SET(Angelscript_SOURCE ${Angelscript_SOURCE} ${Angelscript_${COMPONENT}_SOURCE})
  ENDFOREACH()
ENDIF()

MARK_AS_ADVANCED(
  Angelscript_LIBRARY_DEBUG
  Angelscript_LIBRARY_RELEASE
  Angelscript_INCLUDE_DIR
  Angelscript_ADDON_DIR
)
