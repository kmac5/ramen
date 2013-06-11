// Copyright (c) 2013 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_COLOR_COLOR4_HPP
#define RAMEN_COLOR_COLOR4_HPP

#include<ramen/config.hpp>

#include<cassert>

#include<boost/operators.hpp>

#include<OpenEXR/half.h>

#include<ramen/assert.hpp>

namespace ramen
{
namespace color
{

/*!
\ingroup color
\brief A RGBA color.
*/
template<class T>
class color4_t : boost::equality_comparable<color4_t<T> >
{
public:

    typedef T value_type;

    static unsigned int	dimensions() { return 4;}

    color4_t() {}

    explicit color4_t( T x) : r( x), g( x), b( x), a( x) {}

    color4_t( T rr, T gg, T bb, T aa = T(1)) : r( rr), g( gg), b( bb), a( aa) {}

    T operator()( unsigned int index) const
    {
        RAMEN_ASSERT( index < dimensions());

        return static_cast<const T*>( &r)[index];
    }

    T& operator()( unsigned int index)
    {
        RAMEN_ASSERT( index < dimensions());

        return static_cast<T*>( &r)[index];
    }

    // for regular concept
    bool operator==( const color4_t<T>& other) const
    {
        return  r == other.r &&
                g == other.g &&
                b == other.b &&
                a == other.a;
    }

    T r, g, b, a;
};

// typedefs
typedef color4_t<float>     color4f_t;
typedef color4_t<double>    color4d_t;
typedef color4_t<half>      color4h_t;

} // color
} // ramen

#endif
