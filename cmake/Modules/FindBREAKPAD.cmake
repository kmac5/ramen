# Find google's Breakpad headers and libraries.
#
#  BREAKPAD_INCLUDE_DIRS
#  BREAKPAD_LIBRARY
#  BREAKPAD_CLIENT_LIBRARY
#  BREAKPAD_COMPILE_FLAGS
#  BREAKPAD_FOUND        - True if BREAKPAD found.

# Look for the header files.
find_path( BREAKPAD_INCLUDE_DIR NAMES client/minidump_file_writer.h)

# Look for the library.
find_library( BREAKPAD_LIB NAMES breakpad)
find_library( BREAKPAD_CLIENT_LIB NAMES breakpad_client)

# handle the QUIETLY and REQUIRED arguments and set BREAKPAD_FOUND to TRUE if
# all listed variables are TRUE
include( FindPackageHandleStandardArgs)
find_package_handle_standard_args( BREAKPAD DEFAULT_MSG BREAKPAD_LIB BREAKPAD_CLIENT_LIB BREAKPAD_INCLUDE_DIR)

# Copy the results to the output variables.
set( BREAKPAD_LIBRARY ${BREAKPAD_LIB})
set( BREAKPAD_CLIENT_LIBRARY ${BREAKPAD_CLIENT_LIB})
set( BREAKPAD_INCLUDE_DIRS ${BREAKPAD_INCLUDE_DIR})

mark_as_advanced( BREAKPAD_INCLUDE_DIR BREAKPAD_LIB BREAKPAD_CLIENT_LIB)
