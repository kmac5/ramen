// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_CAMERA_ORTHO_PROJECTION_HPP
#define RAMEN_CAMERA_ORTHO_PROJECTION_HPP

#include<ramen/camera/projection.hpp>

#include<OpenEXR/ImathFrustum.h>

namespace ramen
{
namespace camera
{

class ortho_projection_t : public projection_t
{
public:

	ortho_projection_t() { reset();}

	void reset();

	const Imath::V2d& size() const		{ return size_;}
	void set_size( const Imath::V2d& s)	{ size_ = s;}

	double width() const		{ return size_.x;}
	void set_width( double w)	{ size_.x = w;}

	double height() const		{ return size_.y;}
	void set_height( double h)	{ size_.y = h;}

	Imath::Frustumd frustrum( int w, int h, double pixel_aspect = 1.0) const;

private:

	Imath::V2d size_;
};

} // namespace
} // namespace

#endif
