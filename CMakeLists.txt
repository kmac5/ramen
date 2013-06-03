#####################################################################
# Copyright (C) 2013 Esteban Tovagliari                             #
# Licensed under the terms of the CDDL License.                     #
# See CDDL_LICENSE.txt for a copy of the license.                   #
#####################################################################

CMAKE_MINIMUM_REQUIRED( VERSION 2.8.0 FATAL_ERROR)

######################################################
# project

PROJECT( ramen)

SET( RAMEN_VERSION_MAJOR "0")
SET( RAMEN_VERSION_MINOR "9")
SET( RAMEN_VERSION_PATCH "0")
SET( RAMEN_VERSION ${RAMEN_VERSION_MAJOR}.${RAMEN_VERSION_MINOR}.${RAMEN_VERSION_PATCH})

SET( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake/Modules)

#disable in source builds
IF( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	MESSAGE(FATAL_ERROR "CMake generation for Ramen is not allowed within the source directory!")
ENDIF()

# Set the default built type
IF( NOT CMAKE_BUILD_TYPE)
  SET( CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
ENDIF()

######################################################
# options

OPTION( WITH_TESTS "Build tests" ON)

######################################################
# platform specific

IF( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # Mac OS X
ELSEIF( ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # Linux
ELSEIF( ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # Windows
ELSE()
    MESSAGE( FATAL_ERROR "Platform ${CMAKE_SYSTEM_NAME} not supported yet")
ENDIF()

######################################################
# compiler & linker settings

IF( CMAKE_COMPILER_IS_GNUCXX)
    SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe -fvisibility=hidden -fvisibility-inlines-hidden")
ELSEIF( CMAKE_COMPILER_IS_CLANGXX)
    MESSAGE( FATAL_ERROR "clang is not supported yet")
ELSE()
    MESSAGE( FATAL_ERROR "unknown compiler is not supported yet")
ENDIF()

######################################################
# dependencies

# base libs
SET( Boost_ADDITIONAL_VERSIONS "1.49.0")
SET( BOOST_NEEDED_LIBS  filesystem
                        thread
                        system
                        regex
                        date_time
                        program_options
                        )

FIND_PACKAGE( Boost COMPONENTS ${BOOST_NEEDED_LIBS} REQUIRED)
ADD_DEFINITIONS( -DBOOST_FILESYSTEM_VERSION=3)
ADD_DEFINITIONS( -DBOOST_FILESYSTEM_NO_DEPRECATED)

FIND_PACKAGE( TBB REQUIRED)

# more libraries
FIND_PACKAGE( OpenEXR REQUIRED)
FIND_PACKAGE( JPEG REQUIRED)
FIND_PACKAGE( PNG REQUIRED)
FIND_PACKAGE( TIFF REQUIRED)
FIND_PACKAGE( OpenImageIO REQUIRED)
FIND_PACKAGE( OpenColorIO REQUIRED)
FIND_PACKAGE( ZLIB REQUIRED)

# GL
FIND_PACKAGE( OpenGL REQUIRED)
FIND_PACKAGE( GLEW REQUIRED)

# Qt
SET( QT_MT_REQUIRED false)
SET( QT_MIN_VERSION "4.7.0")
FIND_PACKAGE( Qt4 REQUIRED)
ADD_DEFINITIONS( -DQT_NO_KEYWORDS)
SET( NEEDED_QT_INCLUDES ${QT_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTOPENGL_INCLUDE_DIR})
SET( NEEDED_QT_LIBS	${QT_QTCORE_LIBRARIES} ${QT_QTGUI_LIBRARIES} ${QT_QTOPENGL_LIBRARIES})
SET( MOC_EXTRA_ARGS -DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED)

######################################################
# optional dependencies

######################################################
# definitions

IF( CMAKE_BUILD_TYPE MATCHES Debug)
    ADD_DEFINITIONS( -DRAMEN_ENABLE_ASSERT_HANDLER)
ENDIF()

######################################################
# ramen

INCLUDE_DIRECTORIES( 	${PROJECT_SOURCE_DIR}
                     	${PROJECT_SOURCE_DIR}/ramen/
                        ${NEEDED_QT_INCLUDES}
                        ${Boost_INCLUDE_DIR}
                        ${TBB_INCLUDE_DIRS}
                        ${OPENGL_INCLUDE_DIR}
                        ${GLEW_INCLUDE_DIRS}
                        ${OPENEXR_INCLUDE_DIR}
                        ${OPENEXR_INCLUDE_DIR}/OpenEXR/
                        ${JPEG_INCLUDE_DIR}
                        ${PNG_INCLUDE_DIR}
                        ${TIFF_INCLUDE_DIR}
                        ${OPENIMAGEIO_INCLUDE_DIRS}
                        ${OPENCOLORIO_INCLUDE_DIRS}
                        ${ZLIB_INCLUDE_DIRS}
                        )

SET( RAMEN_ALL_LIBS ${NEEDED_QT_LIBS}
                    ${OPENCOLORIO_LIBRARIES}
                    ${OPENIMAGEIO_LIBRARIES}
                    ${OPENEXR_LIBRARIES}
                    ${JPEG_LIBRARIES}
                    ${PNG_LIBRARIES}
                    ${TIFF_LIBRARIES}
                    ${GLEW_LIBRARIES}
                    ${OPENGL_LIBRARIES}
                    ${Boost_LIBRARIES}
                    ${TBB_LIBRARIES}
                    ${ZLIB_LIBRARIES}
                    )



ADD_SUBDIRECTORY( ramen)

######################################################
# tests

IF( WITH_TESTS)
#    INCLUDE_DIRECTORIES(  google_testing_libs/google
#                          google_testing_libs/google/gtest
#                          google_testing_libs/google/gmock
#                          )

#    ADD_LIBRARY( gmock STATIC   ${PROJECT_SOURCE_DIR}/google_testing_libs/google/gtest/src/gtest-all.cc
#                                ${PROJECT_SOURCE_DIR}/google_testing_libs/google/gmock/src/gmock-all.cc
#                                )

#    ENABLE_TESTING()
ENDIF()

######################################################
# docs

FIND_PACKAGE( Doxygen)

IF( DOXYGEN_FOUND)
	CONFIGURE_FILE( ${PROJECT_SOURCE_DIR}/docs/Doxyfile.in ${PROJECT_BINARY_DIR}/Doxyfile @ONLY)
        ADD_CUSTOM_TARGET(  doc ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
                            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
                            COMMENT "Generating API documentation with Doxygen" VERBATIM)
ENDIF()
