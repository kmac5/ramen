// Copyright (c) 2013 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_MATH_CMATH_HPP
#define RAMEN_MATH_CMATH_HPP

#include<ramen/config.hpp>

#include<math.h>

namespace ramen
{
namespace math
{

template<class T>
struct cmath
{
    static T sqrt( T x)         { return ::sqrt( static_cast<double>( x));}
    static T fabs( T x)         { return ::fabs( static_cast<double>( x));}

    static T sin( T x)          { return ::sin( static_cast<double>( x));}
    static T cos( T x)          { return ::cos( static_cast<double>( x));}
    static T tan( T x)          { return ::tan( static_cast<double>( x));}

    static T exp( T x)          { return ::exp( static_cast<double>( x));}
    static T atan2( T x, T y)   { return ::atan2( static_cast<double>( x), static_cast<double>( y));}

    static T floor( T x)        { return ::floor( x);}
    static T ceil( T x)         { return ::ceil( x);}

    static T pow( T x, T y)     { return ::pow( x, y);}

    static T log10( T x)        { return ::log10( x);}
};

// float specialization.
template<>
struct cmath<float>
{
    static float sqrt( float x)             { return ::sqrtf( x);}
    static float fabs( float x)             { return ::fabsf( x);}

    static float sin( float x)              { return ::sinf( x);}
    static float cos( float x)              { return ::cosf( x);}
    static float tan( float x)              { return ::tanf( x);}

    static float exp( float x)              { return ::expf( x);}
    static float atan2( float x, float y)   { return ::atan2f( x, y);}

    static float floor( float x)            { return ::floorf( x);}
    static float ceil( float x)             { return ::ceilf( x);}

    static float pow( float x, float y)     { return ::powf( x, y);}

    static float log10( float x)            { return ::log10f( x);}
};

} // math
} // ramen

#endif
