// Copyright (c) 2013 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_MATH_HPOINT2_HPP
#define RAMEN_MATH_HPOINT2_HPP

#include<ramen/config.hpp>

#include<ramen/math/point2.hpp>

namespace ramen
{
namespace math
{

/*!
\ingroup math
\brief Homogeneus two dimensional point.
*/
template<class T>
class hpoint2_t : boost::equality_comparable<hpoint2_t<T> >
{
public:

    typedef T value_type;

    static const unsigned int dimensions    = 2;
    static const bool is_homogeneus         = true;
    static unsigned int	size()              { return 3;}

    hpoint2_t() {}

    explicit hpoint2_t( T xx) : x( xx), y( xx), w( 1) {}

    hpoint2_t( T xx, T yy, T ww = T(1)) : x( xx), y( yy), w( ww) {}

    explicit hpoint2_t( const point2_t<T>& p) : x( p.x), y( p.y), w( T(1)) {}

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

    bool operator==( const hpoint2_t<T>& other) const
    {
        return x == other.x && y == other.y && w == other.w;
    }

    T x, y, w;
};

template<class T>
point2_t<T> project( const hpoint2_t<T>& p)
{
    RAMEN_ASSERT( p.w != T(0));

    return point2_t<T>( p.x / p.w, p.y / p.w);
}

// typedefs
typedef hpoint2_t<float>     hpoint2f_t;
typedef hpoint2_t<double>    hpoint2d_t;
typedef hpoint2_t<half>      hpoint2h_t;

} // math
} // ramen

#endif
