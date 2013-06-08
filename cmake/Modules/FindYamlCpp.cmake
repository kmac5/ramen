#  Find YAMLCPP headers and libraries.
#
#  This module defines
#  YAMLCPP_INCLUDE_DIRS - where to find yaml-cpp includes.
#  YAMLCPP_LIBRARIES    - List of libraries when using yaml-cpp.
#  YAMLCPP_FOUND        - True if yaml-cpp found.

# Look for the header file.
FIND_PATH( YAMLCPP_INCLUDE_DIR NAMES yaml-cpp/yaml.h)

# Look for the library.
FIND_LIBRARY( YAMLCPP_LIBRARY NAMES yaml-cpp)

# handle the QUIETLY and REQUIRED arguments and set YAMLCPP_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE( FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS( YAMLCPP DEFAULT_MSG YAMLCPP_LIBRARY YAMLCPP_INCLUDE_DIR)

# our compilation flags
SET( YAMLCPP_COMPILE_FLAGS)

# Copy the results to the output variables.
IF( YAMLCPP_FOUND)
    SET( YAMLCPP_LIBRARIES ${YAMLCPP_LIBRARY})
    SET( YAMLCPP_INCLUDE_DIRS ${YAMLCPP_INCLUDE_DIR} ${YAMLCPP_INCLUDE_DIR}/yaml)
ELSE()
    SET( YAMLCPP_LIBRARIES)
    SET( YAMLCPP_INCLUDE_DIRS)
ENDIF()

MARK_AS_ADVANCED( YAMLCPP_INCLUDE_DIR YAMLCPP_LIBRARY)
