// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_BEZIER_ALGORITHM_HPP
#define	RAMEN_BEZIER_ALGORITHM_HPP

#include<OpenEXR/ImathFun.h>

#include<ramen/bezier/curve.hpp>
#include<ramen/bezier/patch.hpp>

namespace ramen
{
namespace bezier
{

template<class P>
void split_curve( const curve_t<P,3>& c, double t, curve_t<P,3>& a, curve_t<P,3>& b)
{
    typedef typename curve_t<P,3>::point_type point_type;

    point_type p10, p11, p12, p20, p21, p30;

    for( unsigned int j = 0; j < point_type::dimensions(); ++j)
    {
        p10[j] = Imath::lerp( c[0][j], c[1][j], t);
        p11[j] = Imath::lerp( c[1][j], c[2][j], t);
        p12[j] = Imath::lerp( c[2][j], c[3][j], t);

        p20[j] = Imath::lerp( p10[j], p11[j], t);
        p21[j] = Imath::lerp( p11[j], p12[j], t);

        p30[j] = Imath::lerp( p20[j], p21[j], t);
    }

    a[0] = c[0];
    a[1] = p10;
    a[2] = p20;
    a[3] = p30;

    b[0] = p30;
    b[1] = p21;
    b[2] = p12;
    b[3] = c[3];
}

// Optimized for t = 0.5. It's a very common case.
template<class P>
void split_curve( const curve_t<P,3>& c, curve_t<P,3>& a, curve_t<P,3>& b)
{
    typedef typename curve_t<P,3>::point_type   point_type;

    point_type p10, p11, p12, p20, p21, p30;

    double t = 0.5;

    for( unsigned int j = 0; j < point_type::dimensions(); ++j)
    {
        p10[j] = ( c[0][j] + c[1][j]) * t;
        p11[j] = ( c[1][j] + c[2][j]) * t;
        p12[j] = ( c[2][j] + c[3][j]) * t;
        p20[j] = ( p10[j] + p11[j]) * t;
        p21[j] = ( p11[j] + p12[j]) * t;
        p30[j] = ( p20[j] + p21[j]) * t;
    }

    a[0] = c[0];
    a[1] = p10;
    a[2] = p20;
    a[3] = p30;

    b[0] = p30;
    b[1] = p21;
    b[2] = p12;
    b[3] = c[3];
}

Imath::V2f nearest_point_on_curve( const curve_t<Imath::V2f,3>& c, const Imath::V2f& p, float& t);
Imath::V2f nearest_point_on_curve( const curve_t<Imath::V2f,3>& c, const Imath::V2f& p);

// patches

template<class P>
void split_patch_u( const patch_t<P,3>& p, double t, patch_t<P,3>& a, patch_t<P,3>& b)
{
    typedef typename patch_t<P,3>::point_type point_type;
	
    point_type p10, p11, p12, p20, p21, p30;

	for( int j = 0; j < 4; ++j)
	{
	    for( unsigned int k = 0; k < point_type::dimensions(); ++k)
	    {
	        p10[k] = Imath::lerp( p[j][0][k], p[j][1][k], t);
	        p11[k] = Imath::lerp( p[j][1][k], p[j][2][k], t);
	        p12[k] = Imath::lerp( p[j][2][k], p[j][3][k], t);
	        p20[k] = Imath::lerp( p10[k], p11[k], t);
	        p21[k] = Imath::lerp( p11[k], p12[k], t);
	        p30[k] = Imath::lerp( p20[k], p21[k], t);
	    }

	    a[j][0] = p[j][0];
	    a[j][1] = p10;
	    a[j][2] = p20;
	    a[j][3] = p30;

	    b[j][0] = p30;
	    b[j][1] = p21;
	    b[j][2] = p12;
	    b[j][3] = p[j][3];	
	}
}

template<class P>
void split_patch_v( const patch_t<P,3>& p, double t, patch_t<P,3>& a, patch_t<P,3>& b)
{
    typedef typename patch_t<P,3>::point_type point_type;
	
    point_type p10, p11, p12, p20, p21, p30;

	for( int j = 0; j < 4; ++j)
	{
	    for( unsigned int k = 0; k < point_type::dimensions(); ++k)
	    {
	        p10[k] = Imath::lerp( p[0][j][k], p[1][j][k], t);
	        p11[k] = Imath::lerp( p[1][j][k], p[2][j][k], t);
	        p12[k] = Imath::lerp( p[2][j][k], p[3][j][k], t);
	        p20[k] = Imath::lerp( p10[k], p11[k], t);
	        p21[k] = Imath::lerp( p11[k], p12[k], t);
	        p30[k] = Imath::lerp( p20[k], p21[k], t);
	    }

	    a[0][j] = p[0][j];
	    a[1][j] = p10;
	    a[2][j] = p20;
	    a[3][j] = p30;

	    b[0][j] = p30;
	    b[1][j] = p21;
	    b[2][j] = p12;
	    b[3][j] = p[3][j];	
	}
}

// Optimized for u = 0.5. It's a very common case.

template<class P>
void split_patch_u( const patch_t<P,3>& p, patch_t<P,3>& a, patch_t<P,3>& b)
{
    typedef typename patch_t<P,3>::point_type point_type;
	
    double t = 0.5;
	
    point_type p10, p11, p12, p20, p21, p30;

	for( int j = 0; j < 4; ++j)
	{
	    for( unsigned int k = 0; k < point_type::dimensions(); ++k)
	    {
	        p10[k] = ( p[j][0][k] + p[j][1][k]) * t;
	        p11[k] = ( p[j][1][k] + p[j][2][k]) * t;
	        p12[k] = ( p[j][2][k] + p[j][3][k]) * t;
	        p20[k] = ( p10[k] + p11[k]) * t;
	        p21[k] = ( p11[k] + p12[k]) * t;
	        p30[k] = ( p20[k] + p21[k]) * t;
	    }

	    a[j][0] = p[j][0];
	    a[j][1] = p10;
	    a[j][2] = p20;
	    a[j][3] = p30;

	    b[j][0] = p30;
	    b[j][1] = p21;
	    b[j][2] = p12;
	    b[j][3] = p[j][3];	
	}
}

// Optimized for v = 0.5. It's a very common case.

template<class P>
void split_patch_v( const patch_t<P,3>& p, patch_t<P,3>& a, patch_t<P,3>& b)
{
    typedef typename patch_t<P,3>::point_type point_type;

    double t = 0.5;
	
    point_type p10, p11, p12, p20, p21, p30;

	for( int j = 0; j < 4; ++j)
	{
	    for( unsigned int k = 0; k < point_type::dimensions(); ++k)
	    {
	        p10[k] = ( p[0][j][k] + p[1][j][k]) * t;
	        p11[k] = ( p[1][j][k] + p[2][j][k]) * t;
	        p12[k] = ( p[2][j][k] + p[3][j][k]) * t;
	        p20[k] = ( p10[k] + p11[k]) * t;
	        p21[k] = ( p11[k] + p12[k]) * t;
	        p30[k] = ( p20[k] + p21[k]) * t;
	    }

	    a[0][j] = p[0][j];
	    a[1][j] = p10;
	    a[2][j] = p20;
	    a[3][j] = p30;

	    b[0][j] = p30;
	    b[1][j] = p21;
	    b[2][j] = p12;
	    b[3][j] = p[3][j];	
	}
}

} // namespace
} // namespace

#endif
