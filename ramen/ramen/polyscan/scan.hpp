// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_POLYSCAN_SCAN_HPP
#define RAMEN_POLYSCAN_SCAN_HPP

#include<ramen/polyscan/poly.hpp>

#include<algorithm>

#include<OpenEXR/ImathBox.h>
#include<OpenEXR/ImathMath.h>

namespace ramen
{
namespace polyscan
{

void incrementalize_y( int y, const vertex_t& p1, const vertex_t& p2, vertex_t& p, vertex_t& dp);

void incrementalize_x( int x, vertex_t& l, vertex_t& r, const vertex_t& dl, const vertex_t& dr,
					   vertex_t& p, vertex_t& dp);

template<class PixelFun>
void scanline( int y, vertex_t& l, vertex_t& r, const vertex_t& dl, const vertex_t& dr, const Imath::Box2i& window, PixelFun f)
{
    vertex_t p, dp;

	int lx = std::max( (int) Imath::Math<float>::ceil( l.screen.x - 0.5f), window.min.x);
	int rx = std::min( (int) Imath::Math<float>::floor( r.screen.x - 0.5f), window.max.x);

    if( lx > rx)
		return;

    incrementalize_x( lx, l, r, dl, dr, p, dp);

    for( int x = lx; x <= rx; ++x)
	{
		f( x, y, p);
		p.increment( dp);
    }
}

template<class PixelFun>
void scan_convert( const poly_t& p, const Imath::Box2i& window, PixelFun f)
{
	if( p.empty())
		return;

    int li = p.topmost_vertex(), ri = li;
    int rem = p.size();
	int y = Imath::Math<float>::ceil( p[li].screen.y - 0.5f);
    int ly = y - 1, ry = ly;
	int i;

	vertex_t l, r, dl, dr;

    while( rem > 0)
	{
		while( ly <= y && rem > 0)
		{
			--rem;
			i = li - 1;

			if( i < 0)
				i = p.size() - 1;

			incrementalize_y( y, p[li], p[i], l, dl);
			ly = Imath::Math<float>::floor( p[i].screen.y + 0.5f);
			li = i;
		}

		while( ry <= y && rem > 0)
		{
		    --rem;
		    i = ri + 1;

		    if( i >= p.size())
				i = 0;

			incrementalize_y( y, p[ri], p[i], r, dr);
		    ry = Imath::Math<float>::floor( p[i].screen.y + 0.5f);
		    ri = i;
		}

		while( y < ly && y < ry)
	    {
			if( y >= window.min.y && y <= window.max.y)
			{
				if( l.screen.x <= r.screen.x)
					scanline( y, l, r, dl, dr, window, f);
				else
					scanline( y, r, l, dr, dl, window, f);
			}

		    y++;

			l.increment( dl);
			r.increment( dr);
		}
    }
}

} // namespace
} // namespace

#endif
