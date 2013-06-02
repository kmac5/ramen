# - Find gmock
# Find GMOCK headers and libraries.
#
#  GMOCK_INCLUDE_DIRS - where to find GMOCK includes.
#  GMOCK_LIBRARIES    - List of libraries when using GMOCK.
#  GMOCK_COMPILE_FLAGS
#  GMOCK_FOUND        - True if GMOCK found.

# Look for the header file.
FIND_PATH(GMOCK_INCLUDE_DIR NAMES gmock/gmock.h)

# Look for the library.
FIND_LIBRARY(GMOCK_LIBRARY NAMES gmock)

# handle the QUIETLY and REQUIRED arguments and set GMOCK_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GMOCK DEFAULT_MSG GMOCK_LIBRARY GMOCK_INCLUDE_DIR)

# Copy the results to the output variables.
IF(GMOCK_FOUND)
    SET(GMOCK_LIBRARIES ${GMOCK_LIBRARY})
    SET(GMOCK_INCLUDE_DIRS ${GMOCK_INCLUDE_DIR})
ELSE(GMOCK_FOUND)
    SET(GMOCK_LIBRARIES)
    SET(GMOCK_INCLUDE_DIRS)
ENDIF(GMOCK_FOUND)

MARK_AS_ADVANCED(GMOCK_INCLUDE_DIR GMOCK_LIBRARY)
