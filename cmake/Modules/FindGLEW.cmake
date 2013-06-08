#  Find glew
#  Find GLEW headers and libraries.
#
#  This module defines
#  GLEW_INCLUDE_DIRS - where to find glew includes.
#  GLEW_LIBRARIES    - List of libraries when using glew.
#  GLEW_FOUND        - True if glew found.

# Look for the header file.
FIND_PATH( GLEW_INCLUDE_DIR NAMES GL/glew.h)

# Look for the library.
FIND_LIBRARY( GLEW_LIBRARY NAMES GLEW)

# handle the QUIETLY and REQUIRED arguments and set GLEW_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE( FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS( GLEW DEFAULT_MSG GLEW_LIBRARY GLEW_INCLUDE_DIR)

# Copy the results to the output variables.
IF( GLEW_FOUND)
    SET( GLEW_LIBRARIES ${GLEW_LIBRARY})
    SET( GLEW_INCLUDE_DIRS ${GLEW_INCLUDE_DIR})
ELSE()
    SET( GLEW_LIBRARIES)
    SET( GLEW_INCLUDE_DIRS)
ENDIF()

MARK_AS_ADVANCED( GLEW_INCLUDE_DIR GLEW_LIBRARY)
