// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_SSE2_SAMPLERS_HPP
#define	RAMEN_IMAGE_SSE2_SAMPLERS_HPP

#include<ramen/image/generic/samplers.hpp>

namespace ramen
{
namespace image
{
namespace sse2
{

/**********************************************************************************/
	
class bilinear_sampler_t : public generic::sampler_t
{
public:

	bilinear_sampler_t() {}
    bilinear_sampler_t( const const_image_view_t& src);
    bilinear_sampler_t( const Imath::Box2i& src_area, const const_image_view_t& src);

    pixel_t operator()( const vector2_t& p) const;
	pixel_t operator()( const vector2_t& p, const vector2_t& du, const vector2_t& dv) const { return (*this)( p);}
};

/**********************************************************************************/

class catrom_sampler_t : public generic::sampler_t
{
public:

	catrom_sampler_t() {}
    catrom_sampler_t( const const_image_view_t& src);
    catrom_sampler_t( const Imath::Box2i& src_area, const const_image_view_t& src);

    pixel_t operator()( const vector2_t& p) const;
	pixel_t operator()( const vector2_t& p, const vector2_t& du, const vector2_t& dv) const { return (*this)( p);}

protected:

    void weights( float t, float w[4]) const;
};

/**********************************************************************************/

class lanczos3_sampler_t : public generic::sampler_t
{
public:

	lanczos3_sampler_t() {}
    lanczos3_sampler_t( const const_image_view_t& src);
    lanczos3_sampler_t( const Imath::Box2i& src_area, const const_image_view_t& src);

    pixel_t operator()( const vector2_t& p) const;
	pixel_t operator()( const vector2_t& p, const vector2_t& du, const vector2_t& dv) const { return (*this)( p);}

    void weights( int c, float t, float w[10]) const;
};

} // namespace
} // namespace
} // namespace

#endif

