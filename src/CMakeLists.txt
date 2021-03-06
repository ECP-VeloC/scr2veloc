# Header files need not be enumerated
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${PROJECT_BINARY_DIR})

###########
# LIB SCR #
###########

# Install header files
LIST(APPEND libscr_install_headers
	scr.h
)
INSTALL(FILES ${libscr_install_headers} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

LIST(APPEND libscr_srcs
	scr2veloc.c
)

# SCR Library
ADD_LIBRARY(scr_o OBJECT ${libscr_srcs})

ADD_LIBRARY(scr SHARED $<TARGET_OBJECTS:scr_o>)
TARGET_LINK_LIBRARIES(scr ${SCR_EXTERNAL_LIBS})
SET_TARGET_PROPERTIES(scr PROPERTIES OUTPUT_NAME scr CLEAN_DIRECT_OUTPUT 1)
INSTALL(TARGETS scr DESTINATION ${CMAKE_INSTALL_LIBDIR})

ADD_LIBRARY(scr-static STATIC $<TARGET_OBJECTS:scr_o>)
IF(SCR_LINK_STATIC)
        SET_TARGET_PROPERTIES(scr-static PROPERTIES LINK_SEARCH_START_STATIC 1)
        SET_TARGET_PROPERTIES(scr-static PROPERTIES LINK_SEARCH_END_STATIC 1)
ENDIF(SCR_LINK_STATIC)
TARGET_LINK_LIBRARIES(scr-static ${SCR_EXTERNAL_LIBS})

SET_TARGET_PROPERTIES(scr-static PROPERTIES OUTPUT_NAME scr CLEAN_DIRECT_OUTPUT 1)
INSTALL(TARGETS scr-static DESTINATION ${CMAKE_INSTALL_LIBDIR})
