// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_CAMERA_PERSP_PROJECTION_HPP
#define RAMEN_CAMERA_PERSP_PROJECTION_HPP

#include<ramen/camera/projection.hpp>

#include<OpenEXR/ImathFrustum.h>

namespace ramen
{
namespace camera
{

class persp_projection_t : public projection_t
{
public:

	persp_projection_t() { reset();}

	void reset();

    double focal_length() const			{ return focal_length_;}
    void  set_focal_length( double val)	{ focal_length_ = val;}

	const Imath::V2d& apperture() const			{ return apperture_;}
	void set_apperture( const Imath::V2d& a)	{ apperture_ = a;}

    double horizontal_aperture() const			{ return apperture_.x;}
    void set_horizontal_aperture( double val)	{ apperture_.x = val;}

    double vertical_aperture() const		{ return apperture_.y;}
    void set_vertical_aperture( double val)	{ apperture_.y = val;}

	const Imath::V2d& film_offset() const			{ return film_offset_;}
	void set_film_offset( const Imath::V2d& off)	{ film_offset_ = off;}

    double horizontal_film_offset() const			{ return film_offset_.x;}
    void set_horizontal_film_offset( double val)	{ film_offset_.x = val;}

    double vertical_film_offset() const			{ return film_offset_.y;}
    void set_vertical_film_offset( double val)	{ film_offset_.y = val;}

	double field_of_view() const;

	Imath::Frustumd frustrum( int width, int height, double pixel_aspect = 1.0) const;

private:

    double focal_length_;
	Imath::V2d apperture_;
	Imath::V2d film_offset_;
};

} // namespace
} // namespace

#endif
