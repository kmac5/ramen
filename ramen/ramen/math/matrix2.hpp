// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_MATH_MATRIX2_HPP
#define RAMEN_MATH_MATRIX2_HPP

#include<boost/optional.hpp>

#include<OpenEXR/ImathVec.h>
#include<OpenEXR/ImathFun.h>

namespace ramen
{
namespace math
{

// 2x2 determinant
template<class T>
T determinant( T a00, T a01, T a10, T a11)
{ 
	return ( a00 * a11) - ( a01 * a10);
}

template<class T>
boost::optional<Imath::Vec2<T> > solve2x2( T a00, T a01, T a10, T a11, T b0, T b1)
{
	T det = determinant( a00, a01, a10, a11);
	
	if( Imath::iszero( det, (T) 1e-5))
		return boost::optional<Imath::Vec2<T> >();
	
	Imath::Vec2<T> x;
	x.x = ( a11 * b0 - a01 * b1) / det;
	x.y = ( a00 * b1 - a10 * b0) / det;
	return x;
}

} // namespace
} // namespace

#endif
