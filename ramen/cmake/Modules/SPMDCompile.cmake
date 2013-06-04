# ispc compile, based in RSLCompile.
# Esteban Tovagliari
# Original license follows:

# Copyright 2009 Rising Sun Pictures Pty and the other authors and
# contributors. All Rights Reserved.
#
# @author Malcolm Humphreys
#
# @par License:
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
# * Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
# * Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
# * Neither the name of the software's owners nor the names of its
#   contributors may be used to endorse or promote products derived from
#   this software without specific prior written permission.
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# (This is the Modified BSD License)

### Simple Example ###
# build some programs with some external includes
#
#  # compile and install the spmd programs
#  file(GLOB slSources "src/rman/*.ispc")
#  add_spmd_programs(spmdPrograms ${ispcSources}
#      COMPILE_FLAGS ${SHADERLIB_COMPILE_FLAGS} ${CHEESE_COMPILE_FLAGS}
#      STAGE_DESTINATION ${STAGE_EXEC_PREFIX}/programs)
#  install(
#      FILES ${spmdPrograms_OUTPUT}
#      DESTINATION ${INSTALL_EXEC_PREFIX}/programs
#      COMPONENT spmdPrograms)

### DSO Depends Example ###
# builds the programs after the DSO target is built
#
#  # compile and install the spmd programs
#  file(GLOB slSources "src/rman/*.ispc")
#  add_spmd_programs(spmdPrograms ${slSources}
#      COMPILE_FLAGS -DMY_FAV_DEF
#      DEPENDS ${DSO_NAME})
#  install(
#      FILES ${spmdPrograms_OUTPUT}
#      DESTINATION ${INSTALL_EXEC_PREFIX}/programs
#      COMPONENT spmdPrograms)
#  install(
#      FILES ${slSources}
#      DESTINATION ${INSTALL_EXEC_PREFIX}/programs/src
#      COMPONENT spmdPrograms)

# The compiler is found by first looking in the variable ISPC_COMPILER; if
# that's empty, we attempt to fill it in by looking in the path for a program
# called ISPC_COMPILER_NAME
#
set(ISPC_COMPILER_NAME ispc)
set(ISPC_OUTPUT_EXTENSION o)
set(ISPC_COMPILE_FLAGS )

#
# Default verbosity
#
set(ISPC_SETUP_VERBOSE YES)

#
# Defualt stage dir
#
set(ISPC_STAGE_DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/ispc")

# Include PARSE_ARGUMENTS macro
# http://www.itk.org/Wiki/CMakeMacroParseArguments
include(ParseArguments)

#
# Compile spmd programs (.ispc) MACRO
#
# Usage:
# add_spmd_programs(target source1 source2 ...
#   [COMPILER ispc_compiler]
#   [COMPILE_FLAGS flag1 flag2 ...]
#   [DEPENDS target1 target2 ...]
#   [STAGE_DESTINATION stage_path]
#   )
#
# Output:
# A variable ${target}_OUTPUT is created which will contain
# the compiled programs file names.
#
# install(
#     FILES ${myTarget_OUTPUT} ${myTarget_SRC_OUTPUT}
#     DESTINATION ${SOME_SHADER_DIR})

#
MACRO(add_spmd_programs ISPC_TARGET)

    # parse the macro arguments
    PARSE_ARGUMENTS(ISPC_USER
      "COMPILE_FLAGS;STAGE_DESTINATION;DEPENDS;"
      "NOTUSED" ${ARGN})

    # get the list of sources from the args
    set(ISPC_SOURCES ${ISPC_USER_DEFAULT_ARGS})

    if(ISPC_SETUP_VERBOSE)
      message(STATUS "Setting up ISPC target [${ISPC_TARGET}]")
    endif(ISPC_SETUP_VERBOSE)
    set(_spmdprograms_debug OFF)

    # find the shader compiler
    if(NOT ISPC_COMPILER)
      find_program(ISPC_COMPILER ${ISPC_COMPILER_NAME})
    endif()

    # merge the user compile flags with the build systems ones
    set(ISPC_COMPILE_FLAGS ${ISPC_COMPILE_FLAGS} ${ISPC_USER_COMPILE_FLAGS})

    # include any dependent targets LIBRARY_OUTPUT_DIRECTORY
    set(ISPC_DEPEND_FLAGS)
    foreach(_ispc_dep ${ISPC_USER_DEPENDS})
        get_target_property(_dep ${_ispc_dep} LIBRARY_OUTPUT_DIRECTORY)
        set(ISPC_DEPEND_FLAGS ${ISPC_DEPEND_FLAGS} -I${_dep})
    endforeach()

    # debug
    if(_spmdprograms_debug)
        message(STATUS "  ISPC_COMPILER: ${ISPC_COMPILER}")
        message(STATUS "  ISPC_OUTPUT_EXTENSION: ${ISPC_OUTPUT_EXTENSION}")
        message(STATUS "  ISPC_TARGET: ${ISPC_TARGET}")
        message(STATUS "  ISPC_SOURCES: ${ISPC_SOURCES}")
        message(STATUS "  ISPC_COMPILE_FLAGS: ${ISPC_COMPILE_FLAGS}")
        message(STATUS "  ISPC_DEPENDS: ${ISPC_USER_DEPENDS}")
        message(STATUS "  ISPC_DEPEND_FLAGS: ${ISPC_DEPEND_FLAGS}")
        message(STATUS "  ISPC_USER_STAGE_DESTINATION: ${ISPC_USER_STAGE_DESTINATION}")
        message(STATUS "  ISPC_USER_DESTINATION: ${ISPC_USER_DESTINATION}")
    endif(_spmdprograms_debug)

    # work out the where to build/stage the programs
    set(ISPC_STAGE_PREFIX ${ISPC_STAGE_DESTINATION})
    if(ISPC_USER_STAGE_DESTINATION)
        set(ISPC_STAGE_PREFIX ${ISPC_USER_STAGE_DESTINATION})
    endif(ISPC_USER_STAGE_DESTINATION)
    file(MAKE_DIRECTORY ${ISPC_STAGE_PREFIX})

    # setup commands to compile each shader
    set(${ISPC_TARGET}_OUTPUT)
    foreach(ISPC_SOURCE ${ISPC_SOURCES})
        get_filename_component(ISPC_BASENAME ${ISPC_SOURCE} NAME_WE)
        set(ISPC_PROGRAM "${ISPC_STAGE_PREFIX}/${ISPC_BASENAME}.${ISPC_OUTPUT_EXTENSION}")
        list(APPEND ${ISPC_TARGET}_OUTPUT ${ISPC_PROGRAM})
        # Add a command to compile the shader
		if(NOT MINGW)
			add_custom_command(
				OUTPUT ${ISPC_PROGRAM}
				COMMAND ${ISPC_COMPILER} ${ISPC_COMPILE_FLAGS} ${ISPC_DEPEND_FLAGS}
					-o ${ISPC_PROGRAM} ${ISPC_SOURCE}
				DEPENDS ${ISPC_PROGRAM_DEPENDS}
				COMMENT "Compiling ISPC shader ${ISPC_PROGRAM}"
			)
		else()
			file(TO_NATIVE_PATH "${aqsis_util_location}" aqsis_util_path)
			file(TO_NATIVE_PATH "${aqsis_slcomp_location}" aqsis_slcomp_path)
      set(shared_lib_path "${Boost_LIBRARY_DIRS}")
			get_target_property(aqsl_command ${ISPC_COMPILER} LOCATION)
			add_custom_command(
				OUTPUT ${ISPC_PROGRAM}
				COMMAND ${CMAKE_COMMAND}
				-P ${CMAKE_SOURCE_DIR}/cmake/ispcCompile.cmake
				DEPENDS ${ISPC_SHADER_DEPENDS}
				COMMENT "Compiling ISPC program ${ISPC_SHADER}"
			)
		endif()
        if(ISPC_SETUP_VERBOSE)
          message(STATUS "  ${ISPC_SOURCE}")
        endif(ISPC_SETUP_VERBOSE)
    endforeach()

    # Add a target which depends on all compiled programs so that they'll be built
    # prior to the install stage.
    add_custom_target(${ISPC_TARGET} ALL DEPENDS ${${ISPC_TARGET}_OUTPUT})

ENDMACRO(add_spmd_programs)
