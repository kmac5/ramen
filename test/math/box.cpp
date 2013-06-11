// Copyright (c) 2013 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<gtest/gtest.h>

#include<ramen/math/box2.hpp>
#include<ramen/math/box3.hpp>

using namespace ramen::math;

TEST( Box2, All)
{
}

TEST( Box3, Contruct)
{
    box3i_t box;
    ASSERT_TRUE( box.empty());

    point3i_t p( 3, 4, 5);
    point3i_t q( 7, 11, 25);

    box3i_t y( p);
    ASSERT_EQ( y.min, p);
    ASSERT_EQ( y.max, p);

    box3i_t z( p, q);
    ASSERT_EQ( z.min, p);
    ASSERT_EQ( z.max, q);
}

/*
TEST( Box3, ExtendBy)
{
    box3i_t x;
    ASSERT_TRUE( x.empty());

    point3i_t p( 1, 5, 77);

    x.extend_by( p);
    EXPECT_EQ( x.min, p);
    EXPECT_EQ( x.max, p);
    EXPECT_FALSE( x.empty());

    point3i_t q( 11, 55, 177);
    x.extend_by( q);
    EXPECT_EQ( x.min, p);
    EXPECT_EQ( x.max, q);
    EXPECT_FALSE( x.empty());
}
*/

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest( &argc, argv);
    return RUN_ALL_TESTS();
}
