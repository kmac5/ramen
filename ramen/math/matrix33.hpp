// Copyright (c) 2013 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_MATH_MATRIX33_HPP
#define RAMEN_MATH_MATRIX33_HPP

#include<ramen/config.hpp>

#include<algorithm>

#include<boost/operators.hpp>

#include<ramen/math/hpoint2.hpp>

namespace ramen
{
namespace math
{

/*!
\ingroup math
\brief A 3x3 matrix.
*/
template<class T>
class matrix33_t :  boost::multipliable<matrix33_t<T>,
                    boost::equality_comparable<matrix33_t<T> > >
{
public:

    typedef T           value_type;
    typedef const T*    const_iterator;
    typedef T*          iterator;

    static unsigned int	rows()  { return 3;}
    static unsigned int	cols()  { return 3;}
    static unsigned int	size()  { return 9;}

    matrix33_t() {}

    matrix33_t( T a00, T a01, T a02,
                T a10, T a11, T a12,
                T a20, T a21, T a22)
    {
        data_[0][0] = a00; data_[0][1] = a01; data_[0][2] = a02;
        data_[1][0] = a10; data_[1][1] = a11; data_[1][2] = a12;
        data_[2][0] = a20; data_[2][1] = a21; data_[2][2] = a22;
    }

    matrix33_t( const T *data)
    {
        RAMEN_ASSERT( data);

        std::copy( data, data + size(), &data_[0][0]);
    }

    // iterators
    const_iterator begin() const    { return &data_[0][0];}
    const_iterator end() const      { return begin() + size();}

    iterator begin()    { return &data_[0][0];}
    iterator end()      { return begin() + size();}

    T operator()( unsigned int j, unsigned int i) const
    {
        RAMEN_ASSERT( j < 3);
        RAMEN_ASSERT( i < 3);

        return data_[j][i];
    }

    T& operator()( unsigned int j, unsigned int i)
    {
        RAMEN_ASSERT( j < 3);
        RAMEN_ASSERT( i < 3);

        return data_[j][i];
    }

    // operators
    matrix33_t<T>& operator*=( const matrix33_t<T>& other)
    {
        matrix33_t<T> tmp( zero());

        for( int i = 0; i < 3; i++)
            for( int j = 0; j < 3; j++)
                for( int k = 0; k < 3; k++)
                    tmp( i, j) += data_[i][k] * other( k, j);

        *this = tmp;
        return *this;
    }

    // internal access, const only
    const T *data() const
    {
        return &data_[0][0];
    }

    // for regular concept
    bool operator==( const matrix33_t<T>& other) const
    {
        return std::equal( begin(), end(), other.begin(), other.end());
    }

    static matrix33_t<T> identity()
    {
        return matrix33_t<T>(   1, 0, 0,
                                0, 1, 0,
                                0, 0, 1);
    }

    static matrix33_t<T> zero()
    {
        return matrix33_t<T>(   0, 0, 0,
                                0, 0, 0,
                                0, 0, 0);
    }

private:

    T data_[3][3];
};

template<class T, class S>
vector2_t<S> operator*( const vector2_t<S>& v, const matrix33_t<T>& m)
{
    return vector2_t<S>( v.x * m( 0, 0) + v.y * m( 1, 0),
                         v.x * m( 0, 1) + v.y * m( 1, 1));
}

template<class T, class S>
point2_t<S> operator*( const point2_t<S>& p, const matrix33_t<T>& m)
{
    S w = p.x * m( 0, 2) + p.y * m( 1, 2) + m( 2, 2);
    return point2_t<S>( ( p.x * m( 0, 0) + p.y * m( 1, 0) + m( 2, 0)) / w,
                        ( p.x * m( 0, 1) + p.y * m( 1, 1) + m( 2, 1)) / w);
}

template<class T, class S>
hpoint2_t<S> operator*( const hpoint2_t<S>& p, const matrix33_t<T>& m)
{
    return hpoint2_t<S>( p.x * m( 0, 0) + p.y * m( 1, 0) + p.w * m( 2, 0),
                         p.x * m( 0, 1) + p.y * m( 1, 1) + p.w * m( 2, 1),
                         p.x * m( 0, 2) + p.y * m( 1, 2) + p.w * m( 2, 2));
}

typedef matrix33_t<float>   matrix33f_t;
typedef matrix33_t<double>  matrix33d_t;
typedef matrix33_t<half>    matrix33h_t;

} // math
} // ramen

#endif
