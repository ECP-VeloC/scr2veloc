# Install examples source code

LIST(APPEND example_files
	test_common.c
	test_common.h
	test_api.c
)
INSTALL(FILES ${example_files} DESTINATION ${CMAKE_INSTALL_DATADIR}/scr/examples)

## static linking means we must pass a link line to our examples
IF(SCR_LINK_STATIC)
    IF(NOT "${SCR_LINK_LINE}" STREQUAL "")
        STRING(REPLACE ";" " " SCR_LINK_LINE ${SCR_LINK_LINE})
    ENDIF(NOT "${SCR_LINK_LINE}" STREQUAL "")
ELSE(SCR_LINK_STATIC)
    SET(SCR_LINK_LINE)
ENDIF(SCR_LINK_STATIC)

## Configure & install makefile
CONFIGURE_FILE(makefile.examples.in makefile.examples @ONLY)
SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/makefile.examples.in PROPERTIES GENERATED FALSE)
SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_BINARY_DIR}/makefile.examples PROPERTIES GENERATED TRUE)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/makefile.examples DESTINATION ${CMAKE_INSTALL_DATADIR}/scr/examples RENAME makefile)

# CMAKE PROPER FOR EXAMPLES

## Prefer static?
IF(SCR_LINK_STATIC)
        SET(SCR_LINK_TO scr-static)
ELSE(SCR_LINK_STATIC)
        SET(SCR_LINK_TO scr)
ENDIF(SCR_LINK_STATIC)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

ADD_EXECUTABLE(test_api test_common.c test_api.c)
TARGET_LINK_LIBRARIES(test_api ${SCR_LINK_TO})
