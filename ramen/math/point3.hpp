// Copyright (c) 2013 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_MATH_POINT3_HPP
#define RAMEN_MATH_POINT3_HPP

#include<ramen/config.hpp>

#include<ramen/math/vector3.hpp>
#include<ramen/math/normal.hpp>

namespace ramen
{
namespace math
{

/*!
\ingroup math
\brief Three dimensional point.
*/
template<class T>
class point3_t  : boost::addable2<point3_t<T>, vector3_t<T>
                , boost::subtractable2<point3_t<T>, vector3_t<T>
                , boost::addable2<point3_t<T>, normal_t<T>
                , boost::subtractable2<point3_t<T>, normal_t<T>
                , boost::equality_comparable<point3_t<T>
                > > > > >
{
public:

    typedef T value_type;

    static const unsigned int dimensions    = 3;
    static const bool is_homogeneus         = false;
    static unsigned int	size()              { return 3;}

    point3_t() {}

    explicit point3_t( T xx) : x( xx), y( xx), z( xx) {}

    point3_t( T xx, T yy, T zz) : x( xx), y( yy), z( zz) {}

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

    point3_t<T>& operator+=( const vector3_t<T>& vec)
    {
        x += vec.x;
        y += vec.y;
        z += vec.z;
        return *this;
    }

    point3_t<T>& operator-=( const vector3_t<T>& vec)
    {
        x -= vec.x;
        y -= vec.y;
        z -= vec.z;
        return *this;
    }

    point3_t<T>& operator+=( const normal_t<T>& n)
    {
        x += n.x;
        y += n.y;
        z += n.z;
        return *this;
    }

    point3_t<T>& operator-=( const normal_t<T>& n)
    {
        x -= n.x;
        y -= n.y;
        z -= n.z;
        return *this;
    }

    vector3_t<T> operator-( const point3_t<T>& other) const
    {
        return vector3_t<T>( x - other.x, y - other.y, z - other.z);
    }

    bool operator==( const point3_t<T>& other) const
    {
        return x == other.x && y == other.y && z == other.z;
    }

    static point3_t<T> origin()
    {
        return point3_t<T>( 0);
    }

    T x, y, z;
};

// typedefs
typedef point3_t<float>     point3f_t;
typedef point3_t<double>    point3d_t;
typedef point3_t<int>       point3i_t;
typedef point3_t<half>      point3h_t;

} // math
} // ramen

#endif
