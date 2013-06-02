# - Find Shiboken
# Find Shiboken headers and libraries.
#
#  Shiboken_INCLUDE_DIRS - where to find Shiboken includes.
#  Shiboken_LIBRARIES    - List of libraries when using Shiboken.
#  Shiboken_FOUND        - True if Shiboken found.

FIND_PATH( SHIBOKEN_INCLUDE_DIR NAMES shiboken/shiboken.h)
FIND_LIBRARY( SHIBOKEN_LIBRARY NAMES shiboken-python2.7)

INCLUDE( FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS( SHIBOKEN DEFAULT_MSG SHIBOKEN_LIBRARY SHIBOKEN_INCLUDE_DIR)

# Copy the results to the output variables.
IF( SHIBOKEN_FOUND)
	SET( SHIBOKEN_LIBRARIES ${SHIBOKEN_LIBRARY})
	SET( SHIBOKEN_INCLUDE_DIRS  ${SHIBOKEN_INCLUDE_DIR}
                                ${SHIBOKEN_INCLUDE_DIR}/shiboken)
ELSE( SHIBOKEN_FOUND)
	SET( SHIBOKEN_LIBRARIES)
	SET( SHIBOKEN_INCLUDE_DIRS)
ENDIF( SHIBOKEN_FOUND)

MARK_AS_ADVANCED( SHIBOKEN_INCLUDE_DIR SHIBOKEN_LIBRARY)
