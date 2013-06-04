# - Find Imath
# Find Imath headers and libraries.
#
#  IMATH_INCLUDE_DIRS - where to find IMATH uncludes.
#  IMATH_LIBRARIES    - List of libraries when using IMATH.
#  IMATH_FOUND        - True if IMATH found.

# Look for the header file.
FIND_PATH( IMATH_INCLUDE_DIR NAMES OpenEXR/ImathVec.h)

# Look for the libraries.
FIND_LIBRARY( IMATH_HALF_LIBRARY NAMES Half)
FIND_LIBRARY( IMATH_IEX_LIBRARY NAMES Iex)
FIND_LIBRARY( IMATH_MATH_LIBRARY NAMES Imath)

# handle the QUIETLY and REQUIRED arguments and set IMATH_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE( FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS( IMATH DEFAULT_MSG	IMATH_HALF_LIBRARY
														IMATH_IEX_LIBRARY
														IMATH_MATH_LIBRARY
														IMATH_INCLUDE_DIR
														)
# Copy the results to the output variables.
IF(IMATH_FOUND)
	SET( IMATH_LIBRARIES ${IMATH_HALF_LIBRARY}
							${IMATH_IEX_LIBRARY}
							${IMATH_MATH_LIBRARY}
							)

	SET( IMATH_INCLUDE_DIRS ${IMATH_INCLUDE_DIR})
ELSE(IMATH_FOUND)
	SET(IMATH_LIBRARIES)
	SET(IMATH_INCLUDE_DIRS)
ENDIF(IMATH_FOUND)

MARK_AS_ADVANCED(	IMATH_HALF_LIBRARY
					IMATH_IEX_LIBRARY
					IMATH_MATH_LIBRARY
					IMATH_INCLUDE_DIR
					)
