// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMATHEXT_MATRIX_ALGO_HPP
#define RAMEN_IMATHEXT_MATRIX_ALGO_HPP

#include<boost/optional.hpp>
#include<boost/array.hpp>

#include<OpenEXR/ImathMatrix.h>

/*
#include<Eigen/Core>
#include<Eigen/LU>
*/

namespace Imath
{

template<class T>
boost::optional<Matrix33<T> > inverse( const Matrix33<T>& m)
{
	boost::optional<Matrix33<T> > result;
	
	try
	{
		Matrix33<T> inv_m( m.inverse( true));
		result = inv_m;
	}
	catch( Iex::MathExc& e) {}
	
	return result;
}

template<class T>
boost::optional<Matrix44<T> > inverse( const Matrix44<T>& m)
{
	boost::optional<Matrix44<T> > result;
	
	try
	{
		Matrix44<T> inv_m( m.inverse( true));
		result = inv_m;
	}
	catch( Iex::MathExc& e) {}
	
	return result;
}

template<class T>
bool isAffine( const Matrix33<T>& m)
{
    return m[0][2] == 0 && m[1][2] == 0 && m[2][2] == 1;
}

/*
template<class T>
boost::optional<Matrix33<T> > quadToQuadMatrix( const boost::array<Vec2<T> ,4>& src, const boost::array<Vec2<T> ,4>& dst)
{
    Eigen::Matrix<T, 8, 8> A( Eigen::Matrix<T,8,8>::Zero());
    Eigen::Matrix<T, 8, 1> b, x;

    for( int i = 0; i < 4; ++i)
    {
		A( i, 0) = src[i].x;
		A( i, 1) = src[i].y;
		A( i, 2) = 1;

		A( i, 6) = -src[i].x*dst[i].x;
		A( i, 7) = -src[i].y*dst[i].x;

		A( 4 + i, 3) = src[i].x;
		A( 4 + i, 4) = src[i].y;
		A( 4 + i, 5) = 1;

		A( 4 + i, 6) = -src[i].x*dst[i].y;
		A( 4 + i, 7) = -src[i].y*dst[i].y;

        b[i]     = dst[i].x;
        b[4 + i] = dst[i].y;
    }
	
	Eigen::FullPivLU<Eigen::Matrix<T,8,8> > lu_decomp( A);

	if( lu_decomp.isInvertible())
	{
		x = lu_decomp.solve( b);
        return Matrix33<T>(  x[0], x[3], x[6],
                             x[1], x[4], x[7],
                             x[2], x[5], 1);
	}

	return boost::optional<Matrix33<T> >();
}
*/

} // Imath

#endif
