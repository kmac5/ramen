// Copyright (c) 2013 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_MATH_POINT2_HPP
#define RAMEN_MATH_POINT2_HPP

#include<ramen/config.hpp>

#include<ramen/math/vector2.hpp>

namespace ramen
{
namespace math
{

/*!
\ingroup math
\brief Two dimensional point.
*/
template<class T>
class point2_t  : boost::addable2<point2_t<T>, vector2_t<T>
                , boost::subtractable2<point2_t<T>, vector2_t<T>
                , boost::equality_comparable<point2_t<T>
                > > >
{
public:

    typedef T value_type;

    static const unsigned int dimensions    = 2;
    static const bool is_homogeneus         = false;
    static unsigned int	size()              { return 2;}

    point2_t() {}

    explicit point2_t( T xx) : x( xx), y( xx) {}

    point2_t( T xx, T yy) : x( xx), y( yy) {}

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

    // operators

    point2_t<T>& operator+=( const vector2_t<T>& vec)
    {
        x += vec.x;
        y += vec.y;
        return *this;
    }

    point2_t<T>& operator-=( const vector2_t<T>& vec)
    {
        x -= vec.x;
        y -= vec.y;
        return *this;
    }

    vector2_t<T> operator-( const point2_t<T>& other) const
    {
        return vector2_t<T>( x - other.x, y - other.y);
    }

    bool operator==( const point2_t<T>& other) const
    {
        return x == other.x && y == other.y;
    }

    static point2_t<T> origin()
    {
        return point2_t<T>( 0);
    }

    T x, y;
};

// typedefs
typedef point2_t<int>       point2i_t;
typedef point2_t<float>     point2f_t;
typedef point2_t<double>    point2d_t;
typedef point2_t<half>      point2h_t;

} // math
} // ramen

#endif
