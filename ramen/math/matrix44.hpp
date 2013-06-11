// Copyright (c) 2013 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_MATH_MATRIX44_HPP
#define RAMEN_MATH_MATRIX44_HPP

#include<ramen/config.hpp>

#include<algorithm>

#include<boost/operators.hpp>
#include<boost/optional.hpp>

#include<ramen/math/cmath.hpp>
#include<ramen/math/constants.hpp>
#include<ramen/math/hpoint3.hpp>

namespace ramen
{
namespace math
{

/*!
\ingroup math
\brief A 4x4 matrix.
*/
template<class T>
class matrix44_t :  boost::multipliable<matrix44_t<T>,
                    boost::equality_comparable<matrix44_t<T> > >
{
public:

    typedef T           value_type;
    typedef const T*    const_iterator;
    typedef T*          iterator;

    static unsigned int	rows()  { return 4;}
    static unsigned int	cols()  { return 4;}
    static unsigned int	size()  { return 16;}

    matrix44_t() {}

    matrix44_t( T a00, T a01, T a02, T a03,
                T a10, T a11, T a12, T a13,
                T a20, T a21, T a22, T a23,
                T a30, T a31, T a32, T a33)
    {
        data_[0][0] = a00; data_[0][1] = a01; data_[0][2] = a02; data_[0][3] = a03;
        data_[1][0] = a10; data_[1][1] = a11; data_[1][2] = a12; data_[1][3] = a13;
        data_[2][0] = a20; data_[2][1] = a21; data_[2][2] = a22; data_[2][3] = a23;
        data_[3][0] = a30; data_[3][1] = a31; data_[3][2] = a32; data_[3][3] = a33;
    }

    template<class Iter>
    explicit matrix44_t( Iter it)
    {
        std::copy( it, it + size(), begin());
    }

    T operator()( unsigned int j, unsigned int i) const
    {
        RAMEN_ASSERT( j < rows());
        RAMEN_ASSERT( i < cols());

        return data_[j][i];
    }

    T& operator()( unsigned int j, unsigned int i)
    {
        RAMEN_ASSERT( j < rows());
        RAMEN_ASSERT( i < cols());

        return data_[j][i];
    }

    // iterators
    const_iterator begin() const    { return &data_[0][0];}
    const_iterator end() const      { return begin() + size();}

    iterator begin()    { return &data_[0][0];}
    iterator end()      { return begin() + size();}

    // operators
    matrix44_t<T>& operator*=( const matrix44_t<T>& other)
    {
        matrix44_t<T> tmp( zero_matrix());

        for( unsigned int i = 0; i < rows(); i++)
            for( unsigned int j = 0; j < cols(); j++)
                for( unsigned int k = 0; k < rows(); k++)
                    tmp( i, j) += data_[i][k] * other( k, j);

        *this = tmp;
        return *this;
    }

    void transpose()
    {
        matrix44_t<T> tmp;
        for( int i = 0; i < rows(); i++)
            for( int j = 0; j < cols(); j++)
                tmp( i, j) = (*this)( j, i);

        *this = tmp;
    }

    bool operator==( const matrix44_t<T>& other) const
    {
        return std::equal( begin(), end(), other.begin(), other.end());
    }

    // static factory methods
    static matrix44_t<T> identity_matrix()
    {
        return matrix44_t<T>(   1, 0, 0, 0,
                                0, 1, 0, 0,
                                0, 0, 1, 0,
                                0, 0, 0, 1);
    }

    static matrix44_t<T> zero_matrix()
    {
        return matrix44_t<T>(   0, 0, 0, 0,
                                0, 0, 0, 0,
                                0, 0, 0, 0,
                                0, 0, 0, 0);
    }

    static matrix44_t<T> translation_matrix( const vector3_t<T>& t)
    {
        return matrix44_t<T>(   1  , 0  , 0  , 0,
                                0  , 1  , 0  , 0,
                                0  , 0  , 1  , 0,
                                t.x, t.y, t.z, 1);
    }

    static matrix44_t<T> scale_matrix( T s)
    {
        return matrix44_t<T>(   s, 0, 0, 0,
                                0, s, 0, 0,
                                0, 0, s, 0,
                                0, 0, 0, 1);
    }

    static matrix44_t<T> scale_matrix( const vector3_t<T>& s)
    {
        return matrix44_t<T>(   s.x, 0  , 0  , 0,
                                0  , s.y, 0  , 0,
                                0  , 0  , s.z, 0,
                                0  , 0  , 0  , 1);
    }

    static matrix44_t<T> axis_angle_rotation_matrix( vector3_t<T> axis, T angle_in_deg)
    {
        vector3_t<T> unit( axis.normalized());
        T angle_in_rad = angle_in_deg * constants<T>::deg2rad();
        T sine   = cmath<T>::sin( angle_in_rad);
        T cosine = cmath<T>::cos( angle_in_rad);

        matrix44_t<T> m;
        m( 0, 0) = unit( 0) * unit( 0) * (1 - cosine) + cosine;
        m( 0, 1) = unit( 0) * unit( 1) * (1 - cosine) + unit( 2) * sine;
        m( 0, 2) = unit( 0) * unit( 2) * (1 - cosine) - unit( 1) * sine;
        m( 0, 3) = 0;

        m( 1, 0) = unit( 0) * unit( 1) * (1 - cosine) - unit( 2) * sine;
        m( 1, 1) = unit( 1) * unit( 1) * (1 - cosine) + cosine;
        m( 1, 2) = unit( 1) * unit( 2) * (1 - cosine) + unit( 0) * sine;
        m( 1, 3) = 0;

        m( 2, 0) = unit( 0) * unit( 2) * (1 - cosine) + unit( 1) * sine;
        m( 2, 1) = unit( 1) * unit( 2) * (1 - cosine) - unit( 0) * sine;
        m( 2, 2) = unit( 2) * unit( 2) * (1 - cosine) + cosine;
        m( 2, 3) = 0;

        m( 3, 0) = 0;
        m( 3, 1) = 0;
        m( 3, 2) = 0;
        m( 3, 3) = 1;
        return m;
    }

    static matrix44_t<T> rotate_x_matrix( T angle_in_deg)
    {
        return axis_angle_rotation_matrix( vector3_t<T>( 1, 0, 0), angle_in_deg);
    }

    static matrix44_t<T> rotate_y_matrix( T angle_in_deg)
    {
        return axis_angle_rotation_matrix( vector3_t<T>( 0, 1, 0), angle_in_deg);
    }

    static matrix44_t<T> rotate_z_matrix( T angle_in_deg)
    {
        return axis_angle_rotation_matrix( vector3_t<T>( 0, 0, 1), angle_in_deg);
    }

    static matrix44_t<T> rotation_matrix( const vector3_t<T>& r)
    {
        T sin_x = cmath<T>::sin( r(0) * constants<T>::deg2rad());
        T cos_x = cmath<T>::cos( r(0) * constants<T>::deg2rad());
        T sin_y = cmath<T>::sin( r(1) * constants<T>::deg2rad());
        T cos_y = cmath<T>::cos( r(1) * constants<T>::deg2rad());
        T sin_z = cmath<T>::sin( r(2) * constants<T>::deg2rad());
        T cos_z = cmath<T>::cos( r(2) * constants<T>::deg2rad());

        matrix44_t<T> result;
        result( 0, 0) =  cos_z * cos_y;
        result( 0, 1) =  sin_z * cos_y;
        result( 0, 2) = -sin_y;
        result( 0, 3) =  0;

        result( 1, 0) = -sin_z * cos_x + cos_z * sin_y * sin_x;
        result( 1, 1) =  cos_z * cos_x + sin_z * sin_y * sin_x;
        result( 1, 2) =  cos_y * sin_x;
        result( 1, 3) =  0;

        result( 2, 0) =  sin_z * sin_x + cos_z * sin_y * cos_x;
        result( 2, 1) = -cos_z * sin_x + sin_z * sin_y * cos_x;
        result( 2, 2) =  cos_y * cos_x;
        result( 2, 3) =  0;

        result( 3, 0) =  0;
        result( 3, 1) =  0;
        result( 3, 2) =  0;
        result( 3, 3) =  1;
        return result;
    }

private:

    T data_[4][4];
};

template<class T, class S>
vector3_t<S> operator*( const vector3_t<S>& v, const matrix44_t<T>& m)
{
    return vector3_t<S>( v.x * m( 0, 0) + v.y * m( 1, 0) + v.z * m( 2, 0),
                         v.x * m( 0, 1) + v.y * m( 1, 1) + v.z * m( 2, 1),
                         v.x * m( 0, 2) + v.y * m( 1, 2) + v.z * m( 2, 2));
}

template<class T, class S>
point3_t<S> operator*( const point3_t<S>& p, const matrix44_t<T>& m)
{
    S w = p.x * m( 0, 3) + p.y * m( 1, 3) + p.z * m( 2, 3) + m( 3, 3);
    return point3_t<S>( (p.x * m( 0, 0) + p.y * m( 1, 0) + p.z * m( 2, 0) + m( 3, 0)) / w,
                        (p.x * m( 0, 1) + p.y * m( 1, 1) + p.z * m( 2, 1) + m( 3, 1)) / w,
                        (p.x * m( 0, 2) + p.y * m( 1, 2) + p.z * m( 2, 2) + m( 3, 2)) / w);
}

template<class T, class S>
hpoint3_t<S> operator*( const hpoint3_t<S>& p, const matrix44_t<T>& m)
{
    return hpoint3_t<S>( p.x * m( 0, 0) + p.y * m( 1, 0) + p.z * m( 2, 0) + p.w * m( 3, 0),
                         p.x * m( 0, 1) + p.y * m( 1, 1) + p.z * m( 2, 1) + p.w * m( 3, 1),
                         p.x * m( 0, 2) + p.y * m( 1, 2) + p.z * m( 2, 2) + p.w * m( 3, 2),
                         p.x * m( 0, 3) + p.y * m( 1, 3) + p.z * m( 2, 3) + p.w * m( 3, 3));
}

template<class T>
matrix44_t<T> transpose( const matrix44_t<T>& m)
{
    matrix44_t<T> x( m);
    x.transpose();
    return x;
}

// Adapted from ILM's Imath library.
template<class T>
boost::optional<matrix44_t<T> > invert_gauss_jordan( const matrix44_t<T>& m)
{
    int i, j, k;
    matrix44_t<T> s;
    matrix44_t<T> t( m);

    // Forward elimination
    for( i = 0; i < 3 ; ++i)
    {
        int pivot = i;
        T pivotsize = t( i, i);

        if( pivotsize < 0)
            pivotsize = -pivotsize;

        for( j = i + 1; j < 4; ++j)
        {
            T tmp = t( j, i);

            if( tmp < 0)
                tmp = -tmp;

            if( tmp > pivotsize)
            {
                pivot = j;
                pivotsize = tmp;
            }
        }

        if( pivotsize == 0)
            return boost::optional<matrix44_t<T> >();

        if( pivot != i)
        {
            for( j = 0; j < 4; ++j)
            {
                T tmp;

                tmp = t( i, j);
                t( i, j) = t( pivot, j);
                t( pivot, j) = tmp;

                tmp = s( i, j);
                s( i, j) = s( pivot, j);
                s( pivot, j) = tmp;
            }
        }

        for( j = i + 1; j < 4; ++j)
        {
            T f = t( j, i) / t( i, i);

            for( k = 0; k < 4; ++k)
            {
                t( j, k) -= f * t( i, k);
                s( j, k) -= f * s( i, k);
            }
        }
    }

    // Backward substitution
    for( i = 3; i >= 0; --i)
    {
        T f;

        if( (f = t( i, i)) == 0)
            return boost::optional<matrix44_t<T> >();

        for(j = 0; j < 4; j++)
        {
            t( i, j) /= f;
            s( i, j) /= f;
        }

        for(j = 0; j < i; j++)
        {
            f = t( j, i);

            for(k = 0; k < 4; k++)
            {
                t( j, k) -= f * t( i, k);
                s( j, k) -= f * s( i, k);
            }
        }
    }

    return s;
}

// Adapted from ILM's Imath library.
template<class T>
boost::optional<matrix44_t<T> > invert( const matrix44_t<T>& m)
{
    if( m( 0,3) != 0 || m( 1,3) != 0 || m( 2,3) != 0 || m( 3,3) != 1)
        return invert_gauss_jordan( m);

    matrix44_t<T> s( m( 1,1) * m( 2,2) - m( 2,1) * m( 1,2),
                     m( 2,1) * m( 0,2) - m( 0,1) * m( 2,2),
                     m( 0,1) * m( 1,2) - m( 1,1) * m( 0,2),
                     0,

                     m( 2,0) * m( 1,2) - m( 1,0) * m( 2,2),
                     m( 0,0) * m( 2,2) - m( 2,0) * m( 0,2),
                     m( 1,0) * m( 0,2) - m( 0,0) * m( 1,2),
                     0,

                     m( 1,0) * m( 2,1) - m( 2,0) * m( 1,1),
                     m( 2,0) * m( 0,1) - m( 0,0) * m( 2,1),
                     m( 0,0) * m( 1,1) - m( 1,0) * m( 0,1),
                     0,

                     0,
                     0,
                     0,
                     1);

    T r = m( 0,0) * s( 0, 0) + m( 0,1) * s( 1, 0) + m( 0,2) * s( 2, 0);

    if( cmath<T>::fabs( r) >= T(1))
    {
        for( int i = 0; i < 3; ++i)
        {
            for( int j = 0; j < 3; ++j)
                s( i, j) /= r;
        }
    }
    else
    {
        T mr = cmath<T>::fabs( r) / std::numeric_limits<T>::min();

        for( int i = 0; i < 3; ++i)
        {
            for( int j = 0; j < 3; ++j)
            {
                if( mr > cmath<T>::fabs( s( i, j)))
                    s( i, j) /= r;
                else
                    return boost::optional<matrix44_t<T> >();
            }
        }
    }

    s( 3, 0) = -m( 3,0) * s( 0, 0) - m( 3,1) * s( 1, 0) - m( 3,2) * s( 2, 0);
    s( 3, 1) = -m( 3,0) * s( 0, 1) - m( 3,1) * s( 1, 1) - m( 3,2) * s( 2, 1);
    s( 3, 2) = -m( 3,0) * s( 0, 2) - m( 3,1) * s( 1, 2) - m( 3,2) * s( 2, 2);
    return s;
}

typedef matrix44_t<float>   matrix44f_t;
typedef matrix44_t<double>  matrix44d_t;
typedef matrix44_t<half>    matrix44h_t;

} // math
} // ramen

#endif
