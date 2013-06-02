// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMATHEXT_LINE_SEGMENT_ALGO_HPP
#define RAMEN_IMATHEXT_LINE_SEGMENT_ALGO_HPP

#include<OpenEXR/ImathBox.h>
#include<OpenEXR/ImathFun.h>

#include<ramen/ImathExt/detail/clipLiangBarsky.hpp>

namespace Imath
{

template<class Vec>
Vec closestPointTo( const Vec& p0, const Vec& p1, const Vec& point, typename Vec::BaseType& t)
{
    typedef typename Vec::BaseType RealType;

	Vec d = p1 - p0;
	RealType l2 = d.length2();

	if( l2 == 0 )
	{
		t = 0;
		return p0;
	}

	t = Imath::clamp( ( point - p0).dot( d) / l2, RealType( 0), RealType( 1));
    return Imath::lerp( p0, p1, t);
}

template<class Vec>
typename Vec::BaseType distanceTo( const Vec& p0, const Vec& p1, const Vec& p)
{
    typename Vec::BaseType t;
    return ( closestPointTo( p0, p1, p, t ) - p).length();
}

template<class T>
bool clipLineSegment( Imath::Vec2<T>& p0, Imath::Vec2<T>& p1, const Imath::Box<Imath::Vec2<T> >& box)
{
    unsigned result = detail::clipLineSegment( &p0.x, &p0.y, &p1.x, &p1.y, box);
    return !(result >= 4);
}

template<class T>
bool intersects( const Imath::Vec2<T>& p0, const Imath::Vec2<T>& p1, const Imath::Box<Imath::Vec2<T> >& box)
{
    Imath::Vec2<T> a( p0);
    Imath::Vec2<T> b( p1);
    return clipLineSegment( p0, p1, box);
}

} // namespace

#endif
