// Copyright (c) 2013 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<gtest/gtest.h>

#include<ramen/core/copy_on_write.hpp>

using namespace ramen::core;

#include"movable.hpp"

TEST( CopyOnWrite, Construct)
{
    copy_on_write_t<int> a;
    ASSERT_TRUE( a.unique_instance());

    copy_on_write_t<int> b;
    ASSERT_TRUE( b.unique_instance());

    copy_on_write_t<int> c( b);
    ASSERT_FALSE( b.unique_instance());
    ASSERT_FALSE( c.unique_instance());
}

TEST( CopyOnWrite, CopyMove)
{
    copy_on_write_t<int> a( 77);
    ASSERT_TRUE( a.unique_instance());

    copy_on_write_t<int> b( a);
    ASSERT_FALSE( a.unique_instance());
    ASSERT_FALSE( b.unique_instance());
    ASSERT_EQ( a.read(), b.read());

    // Move
    movable_t mx( 11);
    copy_on_write_t<movable_t> x( boost::move( mx));
    EXPECT_TRUE( mx.was_moved());
}

TEST( CopyOnWrite, ReadWrite)
{
    copy_on_write_t<int> a( 7);
    copy_on_write_t<int> b( a);
    ASSERT_FALSE( a.unique_instance());
    ASSERT_FALSE( b.unique_instance());

    int x = a.read();
    ASSERT_FALSE( a.unique_instance());

    int y = b.read();
    ASSERT_FALSE( b.unique_instance());
    ASSERT_EQ( x, y);

    b.write() = 11;
    ASSERT_TRUE( a.unique_instance());
    ASSERT_TRUE( b.unique_instance());

    ASSERT_NE( a.read(), b.read());
}

struct not_comparable_t {};

TEST( CopyOnWrite, EnableEquality)
{
    copy_on_write_t<int> comp;
    copy_on_write_t<not_comparable_t> not_comp;
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest( &argc, argv);
    return RUN_ALL_TESTS();
}
