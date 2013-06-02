// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/generic/samplers.hpp>

#include<cmath>

#include<ramen/assert.hpp>

namespace ramen
{
namespace image
{
namespace generic
{
	
sampler_t::sampler_t( const const_image_view_t& src ) : src_( src)
{
    src_area_ = Imath::Box2i( Imath::V2i( 0, 0), Imath::V2i( src.width() - 1, src.height() - 1));
}

sampler_t::sampler_t( const Imath::Box2i& src_area, const const_image_view_t& src ) : src_area_( src_area), src_( src)
{
	RAMEN_ASSERT( !src_area_.isEmpty());
	RAMEN_ASSERT( src_area_.size().x + 1 == src_.width());
	RAMEN_ASSERT( src_area_.size().y + 1 == src_.height());
}

point_sampler_t::point_sampler_t( const const_image_view_t& src) : sampler_t( src) {}
point_sampler_t::point_sampler_t( const Imath::Box2i& src_area, const const_image_view_t& src) : sampler_t( src_area, src) {}

pixel_t point_sampler_t::operator()( const vector2_t& p) const
{
    Imath::V2i pi( math::fast_float_to_int( p.x + ( p.x < 0.0f ? -0.5f : 0.5f)),
                   math::fast_float_to_int( p.y + ( p.y < 0.0f ? -0.5f : 0.5f)));

    if( src_area_.intersects( pi))
        return src_( pi.x - src_area_.min.x, pi.y - src_area_.min.y);
    else
        return pixel_t( 0, 0, 0, 0);
}

pixel_t point_sampler_t::operator()( const vector2_t& p, const vector2_t& du, const vector2_t& dv) const { return (*this)( p);}

} // namespace
} // namespace
} // namespace
