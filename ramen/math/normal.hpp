// Copyright (c) 2013 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_MATH_NORMAL_HPP
#define RAMEN_MATH_NORMAL_HPP

#include<ramen/config.hpp>

#include<ramen/math/vector3.hpp>

namespace ramen
{
namespace math
{

/*!
\ingroup math
\brief Normal vector.
*/
template<class T>
class normal_t      : boost::addable<normal_t<T>
                    , boost::multipliable2<normal_t<T>, T
                    , boost::dividable2<normal_t<T>, T
                    , boost::equality_comparable<normal_t<T>
                    > > > >
{
public:

    typedef T value_type;

    static unsigned int	size()          { return 3;}
    static unsigned int	dimensions()    { return 3;}

    normal_t() {}

    explicit normal_t( T xx) : x( xx), y( xx), z( xx) {}

    normal_t( T xx, T yy, T zz) : x( xx), y( yy), z( zz) {}

    explicit normal_t( const vector3_t<T>& v) : x( v.x), y( v.y), z( v.z) {}

    // operators

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

    normal_t<T>& operator+=( const normal_t<T>& n)
    {
        x += n.x;
        y += n.y;
        return *this;
    }

    normal_t<T>& operator-=( const normal_t<T>& n)
    {
        x -= n.x;
        y -= n.y;
        return *this;
    }

    normal_t<T>& operator*=( T s)
    {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    normal_t<T>& operator/=( T s)
    {
        x /= s;
        y /= s;
        z /= s;
        return *this;
    }

    // for regular concept
    bool operator==( const normal_t<T>& other) const
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
normal_t<T> normalize( const normal_t<T>& n)
{
    normal_t<T> x( n);
    x.normalize();
    return x;
}

template<class T>
T dot( const normal_t<T>& a, const vector3_t<T>& b)
{
    return ( a.x * b.x) + ( a.y * b.y) + ( a.z * b.z);
}

template<class T>
T dot( const vector3_t<T>& a, const normal_t<T>& b)
{
    return ( a.x * b.x) + ( a.y * b.y) + ( a.z * b.z);
}

template<class T>
T dot( const normal_t<T>& a, const normal_t<T>& b)
{
    return ( a.x * b.x) + ( a.y * b.y) + ( a.z * b.z);
}

typedef normal_t<float>     normalf_t;
typedef normal_t<double>    normald_t;
typedef normal_t<half>      normalh_t;

} // math
} // ramen

#endif
