######################################################################
# Copyright (C) 2013 Esteban Tovagliari. All Rights Reserved.        #
# Licensed under the terms of the CDDL License.                      #
# See CDDL_LICENSE.txt for a copy of the license.                    #
######################################################################

ENABLE_TESTING()

INCLUDE_DIRECTORIES(	../
                        ../google_testing_libs/google
                        ../google_testing_libs/google/gtest
                        ../google_testing_libs/google/gmock
                        )

MACRO( ADD_RAMEN_TEST sourcefile)
    ADD_EXECUTABLE( ${sourcefile} ${sourcefile}.cpp)
    TARGET_LINK_LIBRARIES( ${sourcefile}    ramen
                                            ${RAMEN_ALL_LIBS}
                                            gmock
                                            )

    ADD_TEST( ${sourcefile} ${CMAKE_CURRENT_BINARY_DIR}/${sourcefile})
ENDMACRO()

ADD_RAMEN_TEST( name)
