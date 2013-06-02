// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_CAMERA_SYNTHEYES_LENS_DISTORTION_HPP
#define RAMEN_CAMERA_SYNTHEYES_LENS_DISTORTION_HPP

#include<OpenEXR/ImathVec.h>
#include<OpenEXR/ImathBox.h>

namespace ramen
{
namespace camera
{
namespace detail
{
	
class syntheyes_distortion_base
{
public:

	syntheyes_distortion_base( float k, float k3, const Imath::Box2i& format, float pixel_aspect);
	
protected:
	
	Imath::V2f transform_to_uv( const Imath::V2f& p) const;
	Imath::V2f transform_from_uv( const Imath::V2f& uv) const;

	Imath::Box2i format_;
	Imath::V2f format_aspect_;
	float pixel_aspect_;
	float k_, k3_;
};

} // detail

class syntheyes_quadratic_undistort : public detail::syntheyes_distortion_base
{
public:

	syntheyes_quadratic_undistort( float k, const Imath::Box2i& format, float pixel_aspect);
	Imath::V2f operator()( const Imath::V2f& p) const;
	Imath::V2f operator()( const Imath::V2i& p) const { return (*this)( Imath::V2f( p.x, p.y));}	
};

class syntheyes_undistort : public detail::syntheyes_distortion_base
{
public:

	syntheyes_undistort( float k, float k3, const Imath::Box2i& format, float pixel_aspect);
	Imath::V2f operator()( const Imath::V2f& p) const;
	Imath::V2f operator()( const Imath::V2i& p) const { return (*this)( Imath::V2f( p.x, p.y));}	
};

class syntheyes_quadratic_redistort : public detail::syntheyes_distortion_base
{
public:

	syntheyes_quadratic_redistort( float k, const Imath::Box2i& format, float pixel_aspect);
	Imath::V2f operator()( const Imath::V2f& p) const;	
	Imath::V2f operator()( const Imath::V2i& p) const { return (*this)( Imath::V2f( p.x, p.y));}
};

class syntheyes_redistort : public detail::syntheyes_distortion_base
{
public:

	syntheyes_redistort( float k, float k3, const Imath::Box2i& format, float pixel_aspect);
	Imath::V2f operator()( const Imath::V2f& p) const;	
	Imath::V2f operator()( const Imath::V2i& p) const { return (*this)( Imath::V2f( p.x, p.y));}
};

} // namespace
} // namespace

#endif
