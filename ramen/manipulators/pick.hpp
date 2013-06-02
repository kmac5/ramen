// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_MANIPULATORS_PICK_HPP
#define RAMEN_MANIPULATORS_PICK_HPP

#include<OpenEXR/ImathBox.h>

namespace ramen
{
namespace manipulators
{
	
int pick_distance();
	
bool inside_pick_distance( const Imath::V2f& p, const Imath::V2f& q, float pixel_scale);
bool inside_pick_distance( const Imath::V2f& p, const Imath::V2f& p0, const Imath::V2f& p1, float pixel_scale);

enum pick_axes_result
{
	axes_no_pick = 0,
	axes_center_picked, 
	axes_x_picked,
	axes_y_picked
};

pick_axes_result pick_xy_axes( const Imath::V2f& p, const Imath::V2f& c, const Imath::V2f& x, const Imath::V2f& y, float xscale, float pixel_scale);
pick_axes_result pick_xy_axes( const Imath::V2f& p, const Imath::V2f& c, float xsize, float ysize, float angle, float xscale, float pixel_scale);

enum pick_box_result
{
	box_no_pick = -1,
	box_topleft_corner_picked,
	box_topright_corner_picked,
	box_botleft_corner_picked,
	box_botright_corner_picked,
	
	box_top_edge_picked,
	box_left_edge_picked,
	box_bot_edge_picked,
	box_right_edge_picked	
};

pick_box_result pick_box( const Imath::V2f& p, const Imath::Box2i& box, float pixel_scale, bool pick_edges = true);
pick_box_result pick_box( const Imath::V2f& p, const Imath::Box2f& box, float pixel_scale, bool pick_edges = true);

} // namespace
} // namespace

#endif
