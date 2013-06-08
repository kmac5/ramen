// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<gtest/gtest.h>

#include<ramen/name.hpp>

using namespace ramen;

name_t global_name( "global_name");

TEST( Name, Basic)
{
    name_t a;
    name_t b( "");
    ASSERT_EQ( a.c_str(), b.c_str());
    EXPECT_TRUE( a.empty());
    EXPECT_TRUE( b.empty());

    name_t p( "P");
    name_t q( "Q");
    ASSERT_NE( p.c_str(), q.c_str());

    name_t p2( "P");
    ASSERT_EQ( p.c_str(), p2.c_str());

    swap( p, q);
    ASSERT_EQ( q.c_str(), p2.c_str());

    name_t X( "");
    ASSERT_EQ( a.c_str(), X.c_str());
    ASSERT_EQ( b.c_str(), X.c_str());

    name_t global_name2( "global_name");
    ASSERT_EQ( global_name, global_name2);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest( &argc, argv);
    return RUN_ALL_TESTS();
}
