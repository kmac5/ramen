// Copyright (c) 2013 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<gtest/gtest.h>

#include<cstring>

#include<ramen/math/matrix33.hpp>
#include<ramen/math/matrix44.hpp>

using namespace ramen::math;

TEST( Matrix44, Construct)
{
    matrix44_t<int> m(  0, 1, 2, 3,
                        4, 5, 6, 7,
                        8, 9, 10, 11,
                        12, 13, 14, 15);

    int m_data[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    ASSERT_EQ( memcmp( (void*) m.begin(), (void*) &m_data[0], 16), 0);

    matrix44_t<int> n( m_data);
    ASSERT_EQ( memcmp( (void*) m.begin(), (void*) &m_data[0], 16), 0);
}

TEST( Matrix44, MatrixMult)
{
    matrix44_t<int> n( 10, 5, 12, 15,
                        5, 19, 2, 16,
                        4, 9, 17, 9,
                       16, 17, 2, 5);

    matrix44_t<int> m(  9,    3,    9,   12,
                       17,   12,    3,    1,
                        5,    0,    9,   18,
                        2,   18,    5,   11);


    matrix44_t<int> mn = m * n;

    int mn_data[16] = { 333, 387, 291, 324, 258, 357, 281, 479, 374, 412, 249, 246, 306, 584, 167, 418 };
    ASSERT_EQ( memcmp( (void*) mn.begin(), (void*) &mn_data[0], 16), 0);

    matrix44_t<int> nm( n * m);
    int nm_data[16] = { 265,   360,   288,   506,
                        410,   531,   200,   291,
                        292,   282,   261,   462,
                        453,   342,   238,   300};
    ASSERT_EQ( memcmp( (void*) nm.begin(), (void*) &nm_data[0], 16), 0);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest( &argc, argv);
    return RUN_ALL_TESTS();
}
