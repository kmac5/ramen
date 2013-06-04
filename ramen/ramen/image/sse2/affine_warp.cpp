// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/sse2/affine_warp.hpp>

#include<ramen/image/generic/affine_warp.hpp>
#include<ramen/image/sse2/samplers.hpp>

namespace ramen
{
namespace image
{
namespace sse2
{

void affine_warp_bilinear( const Imath::Box2i& src_area, const const_image_view_t& src,
		     const Imath::Box2i& dst_area, const image_view_t& dst,
		     const matrix3_t& xform, const matrix3_t& inv_xform)
{
	sse2::bilinear_sampler_t s( src_area, src);
    generic::affine_warp<sse2::bilinear_sampler_t>( s, src_area, dst_area, dst, xform, inv_xform);
}

void affine_warp_catrom( const Imath::Box2i& src_area, const const_image_view_t& src,
		     const Imath::Box2i& dst_area, const image_view_t& dst,
		     const matrix3_t& xform, const matrix3_t& inv_xform)
{
	sse2::catrom_sampler_t s( src_area, src);
    generic::affine_warp<sse2::catrom_sampler_t>( s, src_area, dst_area, dst, xform, inv_xform);
}

// non-black
void affine_warp_bilinear_tile( const Imath::Box2i& src_area, const const_image_view_t& src,
		     const Imath::Box2i& dst_area, const image_view_t& dst,
		     const matrix3_t& xform, const matrix3_t& inv_xform)
{
	sse2::bilinear_sampler_t s( src_area, src);
	generic::tile_sampler_t<sse2::bilinear_sampler_t> ts( s);
    generic::affine_warp_non_black_border_mode<generic::tile_sampler_t<sse2::bilinear_sampler_t> >( ts, dst_area, dst, xform, inv_xform);
}

void affine_warp_bilinear_mirror( const Imath::Box2i& src_area, const const_image_view_t& src,
		     const Imath::Box2i& dst_area, const image_view_t& dst,
		     const matrix3_t& xform, const matrix3_t& inv_xform)
{
	sse2::bilinear_sampler_t s( src_area, src);
	generic::mirror_sampler_t<sse2::bilinear_sampler_t> ts( s);
    generic::affine_warp_non_black_border_mode<generic::mirror_sampler_t<sse2::bilinear_sampler_t> >( ts, dst_area, dst, xform, inv_xform);
}

} // namespace
} // namespace
} // namespace
