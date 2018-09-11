PROJECT(SCR2VELOC)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# Mac rpath Policy
IF(POLICY CMP0042)
          CMAKE_POLICY(SET CMP0042 NEW)
ENDIF(POLICY CMP0042)
SET(CMAKE_MACOSX_RPATH ON)

# RPATH magic from:
# https://gitlab.kitware.com/cmake/community/wikis/doc/cmake/RPATH-handling
# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib64")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# the RPATH to be used when installing, but only if it's not a system directory
#LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
#IF("${isSystemDir}" STREQUAL "-1")
#   SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
#ENDIF("${isSystemDir}" STREQUAL "-1")


SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Configuration Options

# Find Packages & Files

LIST(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

## MPI
INCLUDE(SetupMPI)
IF(MPI_C_FOUND)
        INCLUDE_DIRECTORIES(${MPI_C_INCLUDE_PATH})
        LIST(APPEND SCR_EXTERNAL_LIBS ${MPI_C_LIBRARIES})
ELSE(MPI_C_FOUND)
        MESSAGE("WARNING: Could not find MPI!")
        MESSAGE("         Either add an MPI compiler to your path (using modules)")
        MESSAGE("         Or force CMake to build using the correct compiler (`export CC=mpicc`)")
ENDIF(MPI_C_FOUND)

## VELOC
FIND_PACKAGE(VELOC REQUIRED)
SET(HAVE_VELOC TRUE)
INCLUDE_DIRECTORIES(${VELOC_INCLUDE_DIRS})
LIST(APPEND SCR_EXTERNAL_LIBS ${VELOC_LIBRARIES})

## KVTREE
FIND_PACKAGE(KVTree REQUIRED)
SET(HAVE_KVTREE TRUE)
INCLUDE_DIRECTORIES(${KVTREE_INCLUDE_DIRS})
LIST(APPEND SCR_EXTERNAL_LIBS ${KVTREE_LIBRARIES})

INCLUDE(GNUInstallDirs)
## Use X_ variable names for CLI scripts
## could use CMAKE_INSTALL_FULL_ names instead
SET(X_BINDIR ${CMAKE_INSTALL_FULL_BINDIR} CACHE INTERNAL "bin")
SET(X_DATADIR ${CMAKE_INSTALL_FULL_DATADIR} CACHE INTERNAL "share")
SET(X_INCLUDEDIR ${CMAKE_INSTALL_FULL_INCLUDEDIR} CACHE INTERNAL "include")
SET(X_LIBDIR ${CMAKE_INSTALL_FULL_LIBDIR} CACHE INTERNAL "lib")

# Subdirectories
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/src)
ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(examples)

# Generate config.h with all our build #defs
#CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/cmake/config.h.in ${PROJECT_BINARY_DIR}/config.h)
#SET_SOURCE_FILES_PROPERTIES(config.h.in PROPERTIES GENERATED FALSE)
#SET_SOURCE_FILES_PROPERTIES(${PROJECT_BINARY_DIR}/config.h PROPERTIES GENERATED TRUE)

# Package
#SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Scalable Checkpoint / Restart Library")
#SET(CPACK_PACKAGE_VENDOR "Lawrence Livermore National Laboratory")
#SET(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.md")
#SET(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE.TXT")
## SET(CPACK_PACKAGE_VERSION_MAJOR ${SCR_MAJOR_VERSION})
## SET(CPACK_PACKAGE_VERSION_MINOR ${SCR_MINOR_VERSION})
## SET(CPACK_PACKAGE_VERSION_PATCH ${SCR_PATCH_VERSION})
## SET(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
## IF(WIN32 AND NOT UNIX)
##   # There is a bug in NSI that does not handle full unix paths properly. Make
##   # sure there is at least one set of four (4) backlasshes.
##   SET(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
##   SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
##   SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} My Famous Project")
##   SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.my-project-home-page.org")
##   SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.my-personal-home-page.com")
##   SET(CPACK_NSIS_CONTACT "me@my-personal-home-page.com")
##   SET(CPACK_NSIS_MODIFY_PATH ON)
## ELSE(WIN32 AND NOT UNIX)
##   SET(CPACK_STRIP_FILES "bin/MyExecutable")
##   SET(CPACK_SOURCE_STRIP_FILES "")
## ENDIF(WIN32 AND NOT UNIX)
## SET(CPACK_PACKAGE_EXECUTABLES "MyExecutable" "My Executable")
## INCLUDE(CPACK)
