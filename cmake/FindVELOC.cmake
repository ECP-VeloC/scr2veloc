# - Try to find libveloc
# Once done this will define
#  VELOC_FOUND - System has libveloc
#  VELOC_INCLUDE_DIRS - The libveloc include directories
#  VELOC_LIBRARIES - The libraries needed to use libveloc

FIND_PATH(WITH_VELOC_PREFIX
    NAMES include/veloc.h
)

FIND_LIBRARY(VELOC_CLIENT
    NAMES veloc-client
    HINTS ${WITH_VELOC_PREFIX}/lib
)

FIND_LIBRARY(VELOC_MODULES
    NAMES veloc-modules
    HINTS ${WITH_VELOC_PREFIX}/lib
)

SET(VELOC_LIBRARIES ${VELOC_CLIENT} ${VELOC_MODULES})

FIND_PATH(VELOC_INCLUDE_DIRS
    NAMES veloc.h
    HINTS ${WITH_VELOC_PREFIX}/include
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(VELOC DEFAULT_MSG
    VELOC_LIBRARIES
    VELOC_INCLUDE_DIRS
)
