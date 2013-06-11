// Copyright (c) 2013 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<gtest/gtest.h>

#include<ramen/math/hpoint2.hpp>
#include<ramen/math/hpoint3.hpp>

using namespace ramen::math;

TEST( Point, Construct)
{
    point3f_t p( 7.3f, 1.1f, 4.2f);
    ASSERT_EQ( p(0), 7.3f);
    ASSERT_EQ( p(1), 1.1f);
    ASSERT_EQ( p(2), 4.2f);

    hpoint3f_t q( 7.3f, 1.1f, 4.2f);
    ASSERT_EQ( q(0), 7.3f);
    ASSERT_EQ( q(1), 1.1f);
    ASSERT_EQ( q(2), 4.2f);
    ASSERT_EQ( q(3), 1.0f);
}

TEST( HPoint, Construct)
{
    hpoint3f_t p( 7.3f, 1.1f, 4.2f);
    hpoint3f_t q( 0.53f, 7.1f, 2.0f);
}

TEST( Point, )
{
}

/*
BOOST_AUTO_TEST_CASE( point2_test)
{
    point2f_t p;
}

BOOST_AUTO_TEST_CASE( point2h_test)
{
    point2hf_t p;
}

BOOST_AUTO_TEST_CASE( point3_test)
{
    point3f_t p;
}

BOOST_AUTO_TEST_CASE( hpoint3f_test)
{
    hpoint3f_t p;
}
*/

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest( &argc, argv);
    return RUN_ALL_TESTS();
}
