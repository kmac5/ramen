// Copyright (c) 2013 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_MATH_VECTOR3_HPP
#define RAMEN_MATH_VECTOR3_HPP

#include<ramen/config.hpp>

#include<limits>

#include<boost/operators.hpp>

#include<OpenEXR/half.h>

#include<ramen/assert.hpp>
#include<ramen/math/cmath.hpp>

namespace ramen
{
namespace math
{

/*!
\ingroup math
\brief Three dimensional vector.
*/
template<class T>
class vector3_t     : boost::addable<vector3_t<T>
                    , boost::subtractable<vector3_t<T>
                    , boost::dividable2<vector3_t<T>, T
                    , boost::multipliable2<vector3_t<T>, T
                    , boost::equality_comparable<vector3_t<T>
                    > > > > >
{
public:

    typedef T value_type;

    static unsigned int	size()          { return 3;}
    static unsigned int	dimensions()    { return 3;}

    vector3_t() {}

    explicit vector3_t( T xx) : x( xx), y( xx), z( xx) {}

    vector3_t( T xx, T yy, T zz) : x( xx), y( yy), z( zz) {}

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

    vector3_t<T>& operator+=( const vector3_t<T>& vec)
    {
        x += vec.x;
        y += vec.y;
        z += vec.z;
        return *this;
    }

    vector3_t<T>& operator-=( const vector3_t<T>& vec)
    {
        x -= vec.x;
        y -= vec.y;
        z -= vec.z;
        return *this;
    }

    vector3_t<T>& operator*=( T s)
    {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    vector3_t<T>& operator/=( T s)
    {
        RAMEN_ASSERT( s != T(0));

        x /= s;
        y /= s;
        z /= s;
        return *this;
    }

    vector3_t<T> operator-() const
    {
        return vector3_t<T>( -x, -y, -z);
    }

    bool operator==( const vector3_t<T>& other) const
    {
        return x == other.x && y == other.y && z == other.z;
    }

    T length2() const
    {
        return dot( *this, *this);
    }

    T length() const
    {
        return cmath<T>::sqrt( length2());
    }

    void normalize()
    {
        T l = length();
        RAMEN_ASSERT( l > T(0));

        x /= l;
        y /= l;
        z /= l;
    }

    T x, y, z;
};

template<class T>
vector3_t<T> normalize( const vector3_t<T>& v)
{
    vector3_t<T> x( v);
    x.normalize();
    return x;
}

template<class T>
T dot( const vector3_t<T>& a, const vector3_t<T>& b)
{
    return ( a.x * b.x) + ( a.y * b.y) + ( a.z * b.z);
}

template<class T>
vector3_t<T> cross( const vector3_t<T>& a, const vector3_t<T>& b)
{
    return vector3_t<T>( a.y * b.z - a.z * b.y,
                         a.z * b.x - a.x * b.z,
                         a.x * b.y - a.y * b.x);
}

typedef vector3_t<float>    vector3f_t;
typedef vector3_t<double>   vector3d_t;
typedef vector3_t<half>     vector3h_t;

} // math
} // ramen

#endif
