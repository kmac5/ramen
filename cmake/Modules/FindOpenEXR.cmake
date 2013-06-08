# Find OpenEXR headers and libraries.
#
#  This module defines
# OPENEXR_INCLUDE_DIRS - where to find OPENEXR includes.
# OPENEXR_LIBRARIES    - List of libraries when using OPENEXR.
# OPENEXR_FOUND        - True if OPENEXR found.

# Look for the header file.
FIND_PATH( OPENEXR_INCLUDE_DIR NAMES OpenEXR/ImfHeader.h)

# Look for the libraries.
FIND_LIBRARY( OPENEXR_IMF_LIBRARY NAMES IlmImf)
FIND_LIBRARY( OPENEXR_THREAD_LIBRARY NAMES IlmThread)

# handle the QUIETLY and REQUIRED arguments and set OPENEXR_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE( FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS( OPENEXR DEFAULT_MSG  OPENEXR_IMF_LIBRARY
                                                        OPENEXR_THREAD_LIBRARY
                                                        OPENEXR_INCLUDE_DIR)

# Copy the results to the output variables.
IF( OPENEXR_FOUND)
    SET( OPENEXR_LIBRARIES  ${OPENEXR_IMF_LIBRARY} ${OPENEXR_THREAD_LIBRARY})
    SET( OPENEXR_INCLUDE_DIRS ${OPENEXR_INCLUDE_DIR})
ELSE( OPENEXR_FOUND)
    SET( OPENEXR_LIBRARIES)
    SET( OPENEXR_INCLUDE_DIRS)
ENDIF( OPENEXR_FOUND)

MARK_AS_ADVANCED( OPENEXR_IMF_LIBRARY
                  OPENEXR_INCLUDE_DIR)
