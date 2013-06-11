// Copyright (c) 2013 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<gtest/gtest.h>

#include<ramen/math/vector2.hpp>
#include<ramen/math/vector3.hpp>

using namespace ramen::math;

TEST( Vector2, Construct)
{
    vector2i_t vec( 7, 11);

    int *pvec = reinterpret_cast<int*>( &vec);
    ASSERT_EQ( pvec[0], 7);
    ASSERT_EQ( pvec[1], 11);
}

TEST( Vector2, Indexing)
{
    vector2i_t vec( 7, 11);

    ASSERT_EQ( vec( 0), 7);
    ASSERT_EQ( vec.x, 7);

    ASSERT_EQ( vec( 1), 11);
    ASSERT_EQ( vec.y, 11);
}

TEST( Vector3, Construct)
{
    vector3f_t v;
}

int main( int argc, char **argv)
{
    ::testing::InitGoogleTest( &argc, argv);
    return RUN_ALL_TESTS();
}
