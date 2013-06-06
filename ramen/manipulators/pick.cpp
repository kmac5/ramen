// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/manipulators/pick.hpp>

#include<OpenEXR/ImathMath.h>

#include<ramen/math/constants.hpp>

#include<ramen/ImathExt/ImathLineSegmentAlgo.h>

#include<ramen/app/application.hpp>

#include<ramen/ui/viewer/image_view/image_viewer_strategy.hpp>

namespace ramen
{
namespace manipulators
{

int pick_distance() { return app().preferences().pick_distance();}
	
bool inside_pick_distance( const Imath::V2f& p, const Imath::V2f& q, float pixel_scale)
{
	float d = ( p - q).length() * pixel_scale;
	return d <= pick_distance();
}

bool inside_pick_distance( const Imath::V2f& p, const Imath::V2f& p0, const Imath::V2f& p1, float pixel_scale)
{
    float d = Imath::distanceTo( p0, p1, p) * pixel_scale;
	return d <= pick_distance();
}

pick_axes_result pick_xy_axes( const Imath::V2f& p, const Imath::V2f& c, const Imath::V2f& x, const Imath::V2f& y, float xscale, float pixel_scale)
{
	if( inside_pick_distance( p, c, pixel_scale))
		return axes_center_picked;

	Imath::V2f xx( c + x);
	xx.x *= xscale;	
	
	if( inside_pick_distance( p, c, xx, pixel_scale))
		return axes_x_picked;

	Imath::V2f yy( c + y);
	yy.x *= xscale;
	
	if( inside_pick_distance( p, c, yy, pixel_scale))
		return axes_y_picked;
	
	return axes_no_pick;
}

pick_axes_result pick_xy_axes( const Imath::V2f& p, const Imath::V2f& c, float xsize, float ysize, float angle, float xscale, float pixel_scale)
{
    float cs = Imath::Math<float>::cos( (double) angle * math::constants<double>::deg2rad());
    float ss = Imath::Math<float>::sin( (double) angle * math::constants<double>::deg2rad());
	return pick_xy_axes( p, c, Imath::V2f( cs, ss) * xsize, Imath::V2f( ss, -cs) * ysize, xscale, pixel_scale);
}

pick_box_result pick_box( const Imath::V2f& p, const Imath::Box2i& box, float pixel_scale, bool pick_edges)
{
	return pick_box( p, Imath::Box2f( box.min, box.max), pixel_scale);
}

pick_box_result pick_box( const Imath::V2f& p, const Imath::Box2f& box, float pixel_scale, bool pick_edges)
{
	if( box.isEmpty())
		return box_no_pick;
	
	if( inside_pick_distance( p, box.min, pixel_scale))
		return box_topleft_corner_picked;

	if( inside_pick_distance( p, Imath::V2f( box.max.x, box.min.y), pixel_scale))
		return box_topright_corner_picked;

	if( inside_pick_distance( p, Imath::V2f( box.min.x, box.max.y), pixel_scale))
		return box_botleft_corner_picked;

	if( inside_pick_distance( p, box.max, pixel_scale))
		return box_botright_corner_picked;
	
	if( pick_edges)
	{
		if( inside_pick_distance( p, Imath::V2f( box.min.x, box.min.y), Imath::V2f( box.max.x, box.min.y), pixel_scale))
			return box_top_edge_picked;
	
		if( inside_pick_distance( p, Imath::V2f( box.min.x, box.min.y), Imath::V2f( box.min.x, box.max.y), pixel_scale))
			return box_left_edge_picked;
	
		if( inside_pick_distance( p, Imath::V2f( box.min.x, box.max.y), Imath::V2f( box.max.x, box.max.y), pixel_scale))
			return box_bot_edge_picked;
	
		if( inside_pick_distance( p, Imath::V2f( box.max.x, box.min.y), Imath::V2f( box.max.x, box.max.y), pixel_scale))
			return box_right_edge_picked;
	}

	return box_no_pick;
}

} // namespace
} // namespace
