// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/sse2/projective_warp.hpp>

#include<ramen/image/generic/projective_warp.hpp>
#include<ramen/image/sse2/samplers.hpp>

namespace ramen
{
namespace image
{
namespace sse2
{

void projective_warp_bilinear( const Imath::Box2i& src_area, const const_image_view_t& src,
								 const Imath::Box2i& dst_area, const image_view_t& dst,
								 const matrix3_t& xform, const matrix3_t& inv_xform)
{
	sse2::bilinear_sampler_t s( src_area, src);
    generic::projective_warp<sse2::bilinear_sampler_t>( s, dst_area, dst, xform, inv_xform);
}

void projective_warp_catrom( const Imath::Box2i& src_area, const const_image_view_t& src,
							 const Imath::Box2i& dst_area, const image_view_t& dst,
							 const matrix3_t& xform, const matrix3_t& inv_xform)
{
	sse2::catrom_sampler_t s( src_area, src);
    generic::projective_warp<sse2::catrom_sampler_t>( s, dst_area, dst, xform, inv_xform);
}

} // namespace
} // namespace
} // namespace
