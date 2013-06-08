# Find pcre headers and libraries.
#
#  This module defines
#  PCRE_INCLUDE_DIRS - where to find PCRE uncludes.
#  PCRE_LIBRARIES    - List of libraries when using PCRE.
#  PCRE_FOUND        - True if PCRE found.

# Look for the header file.
FIND_PATH( PCRE_INCLUDE_DIR NAMES pcre.h)

# Look for the library.
FIND_LIBRARY( PCRE_LIBRARY NAMES pcre)
FIND_LIBRARY( PCRECPP_LIBRARY NAMES pcrecpp)
#FIND_LIBRARY( PCREPOSIX_LIBRARY NAMES pcreposix)

# handle the QUIETLY and REQUIRED arguments and set PCRE_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE( FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS( PCRE DEFAULT_MSG PCRE_LIBRARY PCRECPP_LIBRARY PCRE_INCLUDE_DIR)

# Copy the results to the output variables.
IF( PCRE_FOUND)
    SET( PCRE_LIBRARIES ${PCRE_LIBRARY} ${PCRECPP_LIBRARY})
    SET( PCRE_INCLUDE_DIRS ${PCRE_INCLUDE_DIR})
ELSE()
    SET( PCRE_LIBRARIES)
    SET( PCRE_INCLUDE_DIRS)
ENDIF()

MARK_AS_ADVANCED( PCRE_INCLUDE_DIR PCRE_LIBRARY PCRECPP_LIBRARY)
