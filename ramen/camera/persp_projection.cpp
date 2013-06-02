// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/camera/persp_projection.hpp>

#include<OpenEXR/ImathMath.h>

namespace ramen
{
namespace camera
{

void persp_projection_t::reset()
{
	projection_t::reset();

	set_focal_length( 35.0); // in mm.
	set_apperture( Imath::V2d( 3.6, 2.4)); // in cm
	set_film_offset( Imath::V2d( 0, 0)); // in cm
}

double persp_projection_t::field_of_view() const
{
	const double rad2deg = 57.29577951308232087679815481410517033381;
    return  Imath::Math<double>::atan( horizontal_aperture() * 10.0 / ( 2.0 * focal_length())) * rad2deg * 2.0;
}

Imath::Frustumd persp_projection_t::frustrum( int width, int height, double pixel_aspect) const
{
	double half_app_x = horizontal_aperture() * 0.5;
	double half_app_y = vertical_aperture() * 0.5;

	double left   = -( 1.0 + horizontal_overscan()) * half_app_x + horizontal_film_offset();
	double right  =  ( 1.0 + horizontal_overscan()) * half_app_x + horizontal_film_offset();
	double top    = -( 1.0 + vertical_overscan())   * half_app_y + vertical_film_offset();
	double bottom =  ( 1.0 + vertical_overscan())   * half_app_y + vertical_film_offset();

	// fit horizontally the screen window.
	double sc = ( right - left) / ( width * pixel_aspect);
	double cy = ( bottom - top) * 0.5;
	top    = cy - ( height / 2.0 * sc);
	bottom = cy + ( height / 2.0 * sc);

	// map the screen window from the focal length to the near clipping plane.
	double focal2near = near_clipping_plane() / ( focal_length() * 0.1);
	left   *= focal2near;
	right  *= focal2near;
	top    *= focal2near;
	bottom *= focal2near;

	return Imath::Frustum<double>( near_clipping_plane(), far_clipping_plane(), left, right, top, bottom);
}

} // namespace
} // namespace
