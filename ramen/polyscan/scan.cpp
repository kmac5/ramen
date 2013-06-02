// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/polyscan/scan.hpp>

#include<OpenEXR/ImathFun.h>

namespace ramen
{
namespace polyscan
{

void incrementalize_y( int y, const vertex_t& p1, const vertex_t& p2, vertex_t& p, vertex_t& dp)
{
    float dy = p2.screen.y - p1.screen.y;

	if( dy == 0)
		dy = 1;

	float frac = y + 0.5f - p1.screen.y;
	dp.sub_scale( p2, p1, 1.0f / dy);

	p = p1;
	p.madd( dp, frac);
}

void incrementalize_x( int x, vertex_t& l, vertex_t& r, const vertex_t& dl, const vertex_t& dr, vertex_t& p, vertex_t& dp)
{
    float dx = r.screen.x - l.screen.x;

	if( dx == 0)
		dx = 1;

	float lnx = l.screen.x + dl.screen.x;
	Imath::V3f lnuv = Imath::V3f( l.uv.x  + dl.uv.x,
								  l.uv.y  + dl.uv.y,
								  l.inv_w + dl.inv_w);

	float rnx = r.screen.x + dr.screen.x;
	Imath::V3f rnuv = Imath::V3f( r.uv.x  + dr.uv.x,
								  r.uv.y  + dr.uv.y,
								  r.inv_w + dr.inv_w);

	l.nyuv = Imath::lerp( lnuv, rnuv, ( l.screen.x - lnx) / ( rnx - lnx));
	r.nyuv = Imath::lerp( lnuv, rnuv, ( r.screen.x - lnx) / ( rnx - lnx));

	float frac = x + 0.5f - l.screen.x;
	dp.sub_scale( r, l, 1.0f / dx);
	p = l;
	p.madd( dp, frac);
}

} // namespace
} // namespace
