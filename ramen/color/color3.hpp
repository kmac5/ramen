// Copyright (c) 2013 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_COLOR_COLOR3_HPP
#define RAMEN_COLOR_COLOR3_HPP

#include<ramen/config.hpp>

#include<algorithm>
#include<iostream>

#include<boost/operators.hpp>

#include<OpenEXR/half.h>

#include<ramen/assert.hpp>

namespace ramen
{
namespace color
{

/*!
\ingroup color
\brief A 3 component color.
*/
template<class T>
class color3_t    : boost::addable<color3_t<T>
                  , boost::subtractable<color3_t<T>
                  , boost::dividable2<color3_t<T>, T
                  , boost::multipliable2<color3_t<T>, T
                  , boost::equality_comparable<color3_t<T>
                  > > > > >
{
public:

    typedef T value_type;

    static unsigned int	dimensions() { return 3;}

    color3_t() {}

    explicit color3_t( T xx) : x( xx), y( xx), z( xx) {}

    color3_t( T xx, T yy, T zz) : x( xx), y( yy), z( zz) {}

    T operator()( unsigned int index) const
    {
        RAMEN_ASSERT( index < dimensions());

        return static_cast<const T*>( &x)[index];
    }

    T& operator()( unsigned int index)
    {
        RAMEN_ASSERT( index < dimensions());

        return static_cast<T*>( &x)[index];
    }

    color3_t<T>& operator+=( const color3_t<T>& c)
    {
        x += c.x;
        y += c.y;
        z += c.z;
        return *this;
    }

    color3_t<T>& operator-=( const color3_t<T>& c)
    {
        x -= c.x;
        y -= c.y;
        z -= c.z;
        return *this;
    }

    color3_t<T>& operator*=( T s)
    {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    color3_t<T>& operator/=( T s)
    {
        RAMEN_ASSERT( s != T(0));

        x /= s;
        y /= s;
        z /= s;
        return *this;
    }

    bool operator==( const color3_t<T>& other) const
    {
        return ( x == other.x && y == other.y && z == other.z);
    }

    void clamp( T lo = T(0), T hi = T(1))
    {
        x = std::max( lo, std::min( hi, x));
        y = std::max( lo, std::min( hi, y));
        z = std::max( lo, std::min( hi, z));
    }

    T luminance() const
    {
        return 0.2126f * x + 0.7152f * y + 0.0722f * z;
    }

    T min_component() const
    {
        return std::min( x, std::min( y, z));
    }

    T max_component() const
    {
        return std::max( x, std::max( y, z));
    }

    T x, y, z;
};

template<class T>
color3_t<T> clamp( const color3_t<T>& c, T lo = T(0), T hi = T(1))
{
    color3_t<T> x( c);
    c.clamp( lo, hi);
    return x;
}

template<class T>
std::ostream& operator<<( std::ostream& os, const color3_t<T>& c)
{
    os << c.x << ", " << c.y << ", " << c.z;
    return os;
}

// typedefs
typedef color3_t<float>     color3f_t;
typedef color3_t<double>    color3d_t;
typedef color3_t<half>      color3h_t;

} // color
} // ramen

#endif
