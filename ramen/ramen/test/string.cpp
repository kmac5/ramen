// Copyright (c) 2012 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/test/test.hpp>

#include<ramen/util/string.hpp>
using namespace ramen::util;

void test_string_valid_identifier()
{
    BOOST_CHECK( !is_string_valid_identifier( std::string()));
    BOOST_CHECK( is_string_valid_identifier( "node"));
    BOOST_CHECK( is_string_valid_identifier( "param"));
}

static bool registered1 = RAMEN_REGISTER_TEST_CASE( test_string_valid_identifier);

void test_string_increment()
{
    std::string str = "node";
    increment_string_number( str);
    BOOST_CHECK( str == "node_2");

    increment_string_number( str);
    BOOST_CHECK( str == "node_3");

    str = "007";
    increment_string_number( str);
    BOOST_CHECK( str == "8");

    str = "007_param";
    increment_string_number( str);
    BOOST_CHECK( str == "007_param_2");

    str = "007_param_3";
    increment_string_number( str);
    BOOST_CHECK( str == "007_param_4");
}

static bool registered2 = RAMEN_REGISTER_TEST_CASE( test_string_increment);
