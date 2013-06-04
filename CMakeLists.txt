PROJECT( ramen)

SET( RAMEN_VERSION_MAJOR "0")
SET( RAMEN_VERSION_MINOR "9")
SET( RAMEN_VERSION_PATCH "0")
SET( RAMEN_VERSION ${RAMEN_VERSION_MAJOR}.${RAMEN_VERSION_MINOR}.${RAMEN_VERSION_PATCH})

CMAKE_MINIMUM_REQUIRED( VERSION 2.8.0 FATAL_ERROR)
SET( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake/Modules)
SET( CMAKE_SKIP_RPATH ON)

#disable in source builds
IF( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	MESSAGE(FATAL_ERROR "CMake generation for Ramen is not allowed within the source directory!")
ENDIF()

####################################################################
# edit the paths if needed

SET( CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH}
						/usr/include/jpeg
						/usr/include/tiff
						)

SET( CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH})

####################################################################
# options

OPTION( WITH_NODES "Build nodes" ON)

####################################################################
# compiler & linker settings

SET( CMAKE_SKIP_RPATH ON)
SET( CMAKE_SKIP_INSTALL_RPATH ON)

# Set the default built type
IF( NOT CMAKE_BUILD_TYPE)
  SET( CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
ENDIF()

IF( CMAKE_BUILD_TYPE MATCHES Release)
	SET( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -rdynamic -pipe -fvisibility=hidden -fvisibility-inlines-hidden -msse -msse2 -mfpmath=sse")
	SET( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -funroll-loops -fexpensive-optimizations -march=core2")
	SET( CMAKE_EXE_LINKER_FLAGS_RELEASE "-Wl,--export-dynamic")
ELSE()
	IF( CMAKE_BUILD_TYPE MATCHES Debug)
		SET( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -rdynamic -pipe -fvisibility=hidden -fvisibility-inlines-hidden -msse -msse2 -mfpmath=sse")
		SET( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-inline")
		SET( CMAKE_EXE_LINKER_FLAGS_DEBUG "-Wl,--export-dynamic")
	ENDIF()
ENDIF()

ADD_DEFINITIONS( -DRAMEN_ENABLE_ASSERT_HANDLER)

####################################################################
# dependencies

# GL
FIND_PACKAGE( OpenGL REQUIRED)
FIND_PACKAGE( GLEW REQUIRED)

# boost
SET( Boost_ADDITIONAL_VERSIONS "1.49.0")
SET( BOOST_NEEDED_LIBS  filesystem
                        thread
                        signals
                        system
                        regex
                        date_time
                        program_options
                        python
                        )

IF( CMAKE_BUILD_TYPE MATCHES Debug)
	SET( BOOST_NEEDED_LIBS ${BOOST_NEEDED_LIBS} unit_test_framework)
ENDIF()

FIND_PACKAGE( Boost COMPONENTS ${BOOST_NEEDED_LIBS} REQUIRED)
ADD_DEFINITIONS( -DBOOST_FILESYSTEM_VERSION=3)
ADD_DEFINITIONS( -DBOOST_FILESYSTEM_NO_DEPRECATED)

# base libs
FIND_PACKAGE( TBB REQUIRED)
FIND_PACKAGE( YamlCpp REQUIRED)

FIND_PACKAGE( ASL REQUIRED)
SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ASL_COMPILE_FLAGS}")

# Qt
SET( QT_MT_REQUIRED false)
SET( QT_MIN_VERSION "4.0.0")
FIND_PACKAGE( Qt4 REQUIRED)
ADD_DEFINITIONS( -DQT_NO_KEYWORDS)
SET( NEEDED_QT_INCLUDES ${QT_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTOPENGL_INCLUDE_DIR})
SET( NEEDED_QT_LIBS	${QT_QTCORE_LIBRARIES} ${QT_QTGUI_LIBRARIES} ${QT_QTOPENGL_LIBRARIES})
SET( MOC_EXTRA_ARGS -DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED)

# imageio libraries
FIND_PACKAGE( OpenEXR REQUIRED)
FIND_PACKAGE( JPEG REQUIRED)
FIND_PACKAGE( PNG REQUIRED)
FIND_PACKAGE( TIFF REQUIRED)
FIND_PACKAGE( OpenImageIO REQUIRED)

# OpenColorIO
FIND_PACKAGE( OpenColorIO REQUIRED)

#FIND_PACKAGE( OpenCV REQUIRED)
# override OpenCV_LIBS as we don't want to link everything
#SET( OpenCV_LIBS	opencv_core
#					opencv_imgproc
#					opencv_features2d
#					opencv_calib3d
#					opencv_flann
#					opencv_video)
#MESSAGE( "OpenCV found, Libs = ${OpenCV_LIBS}")

FIND_PACKAGE( CIMG REQUIRED)

# more libs
FIND_PACKAGE( AGG REQUIRED)
FIND_PACKAGE( EIGEN REQUIRED)
FIND_PACKAGE( Freetype REQUIRED)
FIND_PACKAGE( ZLIB REQUIRED)
FIND_PACKAGE( SEEXPR REQUIRED)

# python
FIND_PACKAGE( PythonLibs REQUIRED)

# testing, debugging, ...
#FIND_PACKAGE( GMOCK REQUIRED)
#FIND_PACKAGE( GLOG REQUIRED)
#FIND_PACKAGE( BREAKPAD REQUIRED)

INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}
                     ${PROJECT_SOURCE_DIR}/ramen/

						${NEEDED_QT_INCLUDES}

						${Boost_INCLUDE_DIR}
						${TBB_INCLUDE_DIRS}
						${ASL_INCLUDE_DIRS}
						${YAMLCPP_INCLUDE_DIRS}

						${OPENGL_INCLUDE_DIR}
						${GLEW_INCLUDE_DIRS}

						${OPENEXR_INCLUDE_DIR}
						${OPENEXR_INCLUDE_DIR}/OpenEXR/

						${JPEG_INCLUDE_DIR}
						${PNG_INCLUDE_DIR}
						${TIFF_INCLUDE_DIR}
						${OPENIMAGEIO_INCLUDE_DIRS}

						${OPENCOLORIO_INCLUDE_DIRS}

						${CIMG_INCLUDE_DIRS}

						${EIGEN_INCLUDE_DIRS}
						${AGG_INCLUDE_DIRS}
						${FREETYPE_INCLUDE_DIRS}

						${SEEXPR_INCLUDE_DIRS}

						${PYTHON_INCLUDE_DIRS}

						${ZLIB_INCLUDE_DIRS}
						${GLOG_INCLUDE_DIRS}
						${GMOCK_INCLUDE_DIRS}
						${BREAKPAD_INCLUDE_DIRS}
						)

SET( RAMEN_ALL_LIBS	${NEEDED_QT_LIBS}

					${OPENCOLORIO_LIBRARIES}

					${OPENIMAGEIO_LIBRARIES}
					${OPENEXR_LIBRARIES}
					${JPEG_LIBRARIES}
					${PNG_LIBRARIES}
					${TIFF_LIBRARIES}

					${GLEW_LIBRARIES}
					${OPENGL_LIBRARIES}

					${ASL_LIBRARIES}
					${Boost_LIBRARIES}
					${TBB_LIBRARIES}
					${YAMLCPP_LIBRARIES}

					${AGG_LIBRARIES}
					${FREETYPE_LIBRARIES}

					${SEEXPR_LIBRARIES}

					${PYTHON_LIBRARIES}

					${ZLIB_LIBRARIES}
					)

ADD_SUBDIRECTORY( ramen)

##########################################################
###
### Docs
###
##########################################################

# Docs
FIND_PACKAGE( Doxygen)

IF( DOXYGEN_FOUND)
	CONFIGURE_FILE( ${PROJECT_SOURCE_DIR}/docs/Doxyfile.in ${PROJECT_BINARY_DIR}/Doxyfile @ONLY)
	ADD_CUSTOM_TARGET( doc ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
						COMMENT "Generating API documentation with Doxygen" VERBATIM)
ENDIF()
