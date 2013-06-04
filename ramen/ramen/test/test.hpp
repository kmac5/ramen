// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_TEST_TEST_HPP
#define RAMEN_TEST_TEST_HPP

#include<boost/function.hpp>
#include<boost/preprocessor/stringize.hpp>

#define BOOST_TEST_DYN_LINK
#include<boost/test/unit_test.hpp>
using namespace boost::unit_test;

// for internal use, better use the following macro.
bool do_register_ramen_test( const char *name, const boost::function<void()>& fun);

#define RAMEN_REGISTER_TEST_CASE( test_function) \
    do_register_ramen_test( BOOST_PP_STRINGIZE( test_function), boost::function<void()>( test_function))

#endif
