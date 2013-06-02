// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_MANIPULATORS_DRAW_HPP
#define RAMEN_MANIPULATORS_DRAW_HPP

#include<OpenEXR/ImathMatrix.h>
#include<OpenEXR/ImathColor.h>
#include<OpenEXR/ImathBox.h>

namespace ramen
{
namespace manipulators
{

// draw
void draw_small_box( const Imath::V2f& p, float size);
	
void draw_cross( const Imath::V2f& p, float xsize, float ysize, const Imath::Color3c& color, float pixel_scale);

void draw_ellipse( const Imath::V2f& center, float xradius, float yradius, const Imath::Color3c& color, 
				   float pixel_scale, int steps = 60);

// call glVertex for each point
void gl_transformed_box( const Imath::Box2i& box, const Imath::M33d& m, float offset = 0.0f);

void draw_bezier_curve( const Imath::V2f& a, const Imath::V2f& b, const Imath::V2f& c, const Imath::V2f& d, 
						const Imath::Color3c& color, float pixel_scale, int steps = 30);

void draw_xy_axes( const Imath::V2f& c, const Imath::V2f& x, const Imath::V2f& y, float xscale,
				   const Imath::Color3c& xcol, const Imath::Color3c& ycol, float pixel_scale);

void draw_xy_axes( const Imath::V2f& c, float xsize, float ysize, float angle, float xscale,
				   const Imath::Color3c& xcol, const Imath::Color3c& ycol, float pixel_scale);

void draw_box( const Imath::Box2i& box, const Imath::Color3c& color, float pixel_scale, bool draw_corners = false);
void draw_box( const Imath::Box2f& box, const Imath::Color3c& color, float pixel_scale, bool draw_corners = false);

// util
float shadow_offset( float pixel_scale);

} // namespace
} // namespace

#endif
