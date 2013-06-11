// Copyright (c) 2013 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_MATH_HPOINT3_HPP
#define RAMEN_MATH_HPOINT3_HPP

#include<ramen/config.hpp>

#include<ramen/math/point3.hpp>

namespace ramen
{
namespace math
{

/*!
\ingroup math
\brief Homogeneous three dimensional point.
*/
template<class T>
class hpoint3_t : boost::equality_comparable<hpoint3_t<T> >
{
public:

    typedef T value_type;

    static const unsigned int dimensions    = 3;
    static const bool is_homogeneus         = true;
    static unsigned int	size()              { return 4;}

    hpoint3_t() {}

    explicit hpoint3_t( T xx) : x( xx), y( xx), z( xx), w( 1) {}

    hpoint3_t( T xx, T yy, T zz, T ww = T(1)) : x( xx), y( yy), z( zz), w( ww) {}

    explicit hpoint3_t( const point3_t<T>& p) : x( p.x), y( p.y), z( p.z), w( T(1)) {}

    T operator()( unsigned int index) const
    {
        RAMEN_ASSERT( index < size());

        return static_cast<const T*>( &x)[index];
    }

    T& operator()( unsigned int index)
    {
        RAMEN_ASSERT( index < size());

        return static_cast<T*>( &x)[index];
    }

    bool operator==( const hpoint3_t<T>& other) const
    {
        return x == other.x && y == other.y && z == other.z && w == other.w;
    }

    T x, y, z, w;
};

template<class T>
point3_t<T> project( const hpoint3_t<T>& p)
{
    RAMEN_ASSERT( p.w != T(0));

    return point3_t<T>( p.x / p.w, p.y / p.w, p.z / p.w);
}

// typedefs
typedef hpoint3_t<float>     hpoint3f_t;
typedef hpoint3_t<double>    hpoint3d_t;
typedef hpoint3_t<half>      hpoint3h_t;

} // math
} // ramen

#endif
