# - Find glog
# Find GLOG headers and libraries.
#
#  GLOG_INCLUDE_DIRS - where to find glog includes.
#  GLOG_LIBRARIES    - List of libraries when using glog.
#  GLOG_COMPILE_FLAGS
#  GLOG_FOUND        - True if glog found.

# Look for the header file.
FIND_PATH(GLOG_INCLUDE_DIR NAMES glog/logging.h)

# Look for the library.
FIND_LIBRARY(GLOG_LIBRARY NAMES glog)

# handle the QUIETLY and REQUIRED arguments and set GLOG_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GLOG DEFAULT_MSG GLOG_LIBRARY GLOG_INCLUDE_DIR)

# Copy the results to the output variables.
IF(GLOG_FOUND)
    SET(GLOG_LIBRARIES ${GLOG_LIBRARY})
    SET(GLOG_INCLUDE_DIRS ${GLOG_INCLUDE_DIR})
ELSE(GLOG_FOUND)
    SET(GLOG_LIBRARIES)
    SET(GLOG_INCLUDE_DIRS)
ENDIF(GLOG_FOUND)

MARK_AS_ADVANCED(GLOG_INCLUDE_DIR GLOG_LIBRARY)
