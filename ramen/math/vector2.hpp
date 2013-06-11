// Copyright (c) 2013 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_MATH_VECTOR2_HPP
#define RAMEN_MATH_VECTOR2_HPP

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
\brief Two dimensional vector.
*/
template<class T>
class vector2_t     : boost::addable<vector2_t<T>
                    , boost::subtractable<vector2_t<T>
                    , boost::dividable2<vector2_t<T>, T
                    , boost::multipliable2<vector2_t<T>, T
                    , boost::equality_comparable<vector2_t<T>
                    > > > > >
{
public:

    typedef T value_type;

    static unsigned int	size()          { return 2;}
    static unsigned int	dimensions()    { return 2;}

    vector2_t() {}

    explicit vector2_t( T xx) : x( xx), y( xx) {}

    vector2_t( T xx, T yy) : x( xx), y( yy) {}

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

    vector2_t<T>& operator+=( const vector2_t<T>& vec)
    {
        x += vec.x;
        y += vec.y;
        return *this;
    }

    vector2_t<T>& operator-=( const vector2_t<T>& vec)
    {
        x -= vec.x;
        y -= vec.y;
        return *this;
    }

    vector2_t<T>& operator*=( T s)
    {
        x *= s;
        y *= s;
        return *this;
    }

    vector2_t<T>& operator/=( T s)
    {
        x /= s;
        y /= s;
        return *this;
    }

    vector2_t<T> operator-() const
    {
        return vector2_t<T>( -x, -y);
    }

    bool operator==( const vector2_t<T>& other) const
    {
        return x == other.x && y == other.y;
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
    }

    T x, y;
};

template<class T>
vector2_t<T> normalize( const vector2_t<T>& v)
{
    vector2_t<T> x( v);
    x.normalize();
    return x;
}

template<class T>
T dot( const vector2_t<T>& a, const vector2_t<T>& b)
{
    return ( a.x * b.x) + ( a.y * b.y);
}

template<class T>
T cross( const vector2_t<T>& a, const vector2_t<T>& b)
{
    return a.x * b.y - a.y * b.x;
}

// typedefs
typedef vector2_t<float>    vector2f_t;
typedef vector2_t<double>   vector2d_t;
typedef vector2_t<int>      vector2i_t;
typedef vector2_t<half>     vector2h_t;

} // math
} // ramen

#endif
