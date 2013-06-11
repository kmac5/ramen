// Copyright (c) 2013 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<gtest/gtest.h>

#include<ramen/math/cmath.hpp>

using namespace ramen::math;

TEST( CMath, All)
{
    float fx = cmath<float>::sqrt( 7.0f);
    double dx = cmath<double>::sqrt( 7.0);

    fx = cmath<float>::atan2( 7.0f, 3.0f);
    dx = cmath<double>::atan2( 7.0, 3.0);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest( &argc, argv);
    return RUN_ALL_TESTS();
}
