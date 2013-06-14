// Copyright (c) 2013 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<gtest/gtest.h>

#include<ramen/core/poly_copyable.hpp>
#include<ramen/core/poly_regular.hpp>

using namespace ramen::core;

TEST( Poly, Construct)
{
    poly_regular_t x;
    EXPECT_TRUE( x.is_empty());

    poly_regular_t y( 7);
    EXPECT_FALSE( y.is_empty());
    EXPECT_EQ( y.cast<int>(), 7);

    poly_regular_t z( 11);
    EXPECT_EQ( z.cast<int>(), 11);
    EXPECT_EQ( y.type_info(), z.type_info());
}

TEST( Poly, Move)
{
}

TEST( Poly, Equality)
{
    poly_regular_t y( 7.0f);
    EXPECT_EQ( y, y);

    poly_regular_t z( 7.0f);
    EXPECT_EQ( y, z);

    poly_regular_t x( string_t( "X"));
    EXPECT_NE( x, y);

    poly_regular_t v;
    poly_regular_t w;
    EXPECT_EQ( v, w);

    EXPECT_NE( x, w);
    EXPECT_NE( y, w);
}

TEST( Poly, Copy)
{
    poly_regular_t y( 7);
    poly_regular_t z( y);
    EXPECT_EQ( y, z);
}

TEST( Poly, Downcast)
{
    poly_regular_t x( 7);
    poly_copyable_t y( poly_cast<poly_copyable_t&>( x));
    poly_regular_t *xx = poly_cast<poly_regular_t*>( &y);
    EXPECT_TRUE( xx);

    poly_copyable_t yy( 11);
    xx = poly_cast<poly_regular_t*>( &yy);
    EXPECT_FALSE( xx);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest( &argc, argv);
    return RUN_ALL_TESTS();
}
