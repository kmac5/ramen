// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/camera/ortho_projection.hpp>

namespace ramen
{
namespace camera
{

void ortho_projection_t::reset()
{
	projection_t::reset();
	size_.x = 1024;
	size_.y = 1024 * 3 / 4;
}

Imath::Frustumd ortho_projection_t::frustrum( int w, int h, double pixel_aspect) const
{
	double half_app_x = width()  * 0.5;
	double half_app_y = height() * 0.5;

	double left   = -( 1.0 + horizontal_overscan()) * half_app_x;
	double right  =  ( 1.0 + horizontal_overscan()) * half_app_x;
	double top    = -( 1.0 + vertical_overscan())   * half_app_y;
	double bottom =  ( 1.0 + vertical_overscan())   * half_app_y;

	// fit horizontally the screen window.
	double sc = ( right - left) / ( w * pixel_aspect);
	double cy = ( bottom - top) * 0.5;
	top    = cy - ( h / 2.0 * sc);
	bottom = cy + ( h / 2.0 * sc);

	return Imath::Frustum<double>( near_clipping_plane(), far_clipping_plane(), left, right, top, bottom, true);
}

} // namespace
} // namespace
