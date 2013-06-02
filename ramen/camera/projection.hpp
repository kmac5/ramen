// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_CAMERA_PROJECTION_HPP
#define RAMEN_CAMERA_PROJECTION_HPP

#include<OpenEXR/ImathVec.h>

namespace ramen
{
namespace camera
{

class projection_t
{
public:

	const Imath::V2d& overscan() const			{ return overscan_;}
	void set_overscan( const Imath::V2d& o)		{ overscan_ = o;}

    double horizontal_overscan() const			{ return overscan_.x;}
    void set_horizontal_overscan( double val)	{ overscan_.x = val;}

    double vertical_overscan() const			{ return overscan_.y;}
    void set_vertical_overscan( double val)		{ overscan_.y = val;}

	void reset_overscan() { overscan_ = Imath::V2d( 0, 0);}

    double near_clipping_plane() const			{ return near_clipping_plane_;}
    void set_near_clipping_plane( double val)	{ near_clipping_plane_ = val;}

    double far_clipping_plane() const			{ return far_clipping_plane_;}
    void set_far_clipping_plane( double val)	{ far_clipping_plane_ = val;}

protected:

	projection_t() { reset();}

	void reset();

private:

	Imath::V2d overscan_;
    double near_clipping_plane_;
	double far_clipping_plane_;
};

} // namespace
} // namespace

#endif
