// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/agg_renderers.hpp>

#include<ramen/algorithm/clamp.hpp>

namespace ramen
{
namespace image
{

void agg_rgba32f_renderer_t::blend_hline( int x1, int y, int x2, const color_type& c, agg::cover_type cover)
{
	using namespace boost::gil;
	
	if(x1 > x2)
	{
		int t = x2;
		x2 = x1;
		x1 = t;
	}
	
	if( y  > ymax() || y < ymin() || x1 > xmax() || x2 < xmin())
		return;
	
	x1 = std::max( x1, xmin());
	x2 = std::min( x2, xmax());
	
	view_type::x_iterator p = view_.row_begin(y) + x1;
	float alpha = cover / 255.0f * get_color( c, alpha_t());
	float inv_alpha = 1.0f - alpha;
	
	int len = x2 - x1 + 1;
	
	do
	{
		get_color( *p, red_t())    = ( get_color( *p, red_t())   * inv_alpha) + ( get_color( c, red_t())   * alpha);
		get_color( *p, green_t())  = ( get_color( *p, green_t()) * inv_alpha) + ( get_color( c, green_t()) * alpha);
		get_color( *p, blue_t())   = ( get_color( *p, blue_t())  * inv_alpha) + ( get_color( c, blue_t())  * alpha);
		
		float a = get_color( *p, alpha_t());
        a = clamp( a + ( get_color( c, alpha_t()) * alpha), 0.0f, 1.0f);
		get_color( *p, alpha_t()) = a;
		++p;
	}
	while (--len);
}

void agg_rgba32f_renderer_t::blend_solid_hspan( int x, int y, int len, const color_type& c, const agg::cover_type* covers)
{
	using namespace boost::gil;
	
	if( y > ymax() || y < ymin())
		return;
	
	if( x < xmin())
	{
		len -= xmin() - x;
		
		if(len <= 0)
			return;
		
		covers += xmin() - x;
		x = xmin();
	}
	
	if(x + len > xmax())
	{
		len = xmax() - x + 1;
		
		if(len <= 0)
			return;
	}
	
	view_type::x_iterator p = view_.row_begin(y) + x;
	
	do
	{
		float alpha = *covers / 255.0f * get_color( c, alpha_t());
		float inv_alpha = 1.0f - alpha;
		get_color( *p, red_t())    = ( get_color( *p, red_t())   * inv_alpha) + ( get_color( c, red_t())   * alpha);
		get_color( *p, green_t())  = ( get_color( *p, green_t()) * inv_alpha) + ( get_color( c, green_t()) * alpha);
		get_color( *p, blue_t())   = ( get_color( *p, blue_t())  * inv_alpha) + ( get_color( c, blue_t())  * alpha);
		
		float a = get_color( *p, alpha_t());
        a = clamp( a + ( get_color( c, alpha_t()) * alpha), 0.0f, 1.0f);
		get_color( *p, alpha_t()) = a;
		
		++p;
		++covers;
	}
	while( --len);
}

} // image
} // ramen
