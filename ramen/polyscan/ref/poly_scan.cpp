/*
 * Generic Convex Polygon Scan Conversion and Clipping
 * by Paul Heckbert
 * from "Graphics Gems", Academic Press, 1990
 */

/*
 * poly_scan.c: point-sampled scan conversion of convex polygons
 *
 * Paul Heckbert	1985, Dec 1989
 */

#include<string.h>

#include"poly_scan.hpp"

void copy_poly( Poly *src, Poly *dst) { memcpy( dst, src, sizeof( Poly));}

/*
 * incrementalize_y: put intersection of line Y=y+.5 with edge between points
 * p1 and p2 in p, put change with respect to y in dp
 */

void incrementalize_y( float *p1, float *p2, float *p, float *dp, int y, unsigned long mask)
{
    float dy, frac;

    dy = ((Poly_vert *)p2)->sy - ((Poly_vert *)p1)->sy;

    if( dy==0.)
		dy = 1.;
	
    frac = y+.5 - ((Poly_vert *)p1)->sy;

    for( ; mask!=0; mask>>=1, p1++, p2++, p++, dp++)
	{
		if( mask & 1)
		{
		    *dp = (*p2-*p1)/dy;
		    *p = *p1+*dp*frac;
		}
	}
}

/*
 * incrementalize_x: put intersection of line X=x+.5 with edge between points
 * p1 and p2 in p, put change with respect to x in dp
 */

void incrementalize_x( float *p1, float *p2, float *p, float *dp, int x, unsigned long mask)
{
    float dx, frac;

    dx = ((Poly_vert *)p2)->sx - ((Poly_vert *)p1)->sx;
	
    if( dx == 0.0)
		dx = 1.0;
	
    frac = x + 0.5 - ( ( Poly_vert *) p1)->sx;

    for (; mask!=0; mask>>=1, p1++, p2++, p++, dp++)
	{
		if (mask&1)
		{
		    *dp = ( *p2 - *p1) / dx;
		    *p = *p1 + *dp * frac;
		}
	}
}

void increment( float *p, float *dp, unsigned long mask)
{
    for( ; mask!=0; mask>>=1, p++, dp++)
	{
		if( mask&1)
		    *p += *dp;
	}
}
