// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_SSE2_AFFINE_WARP_HPP
#define RAMEN_IMAGE_SSE2_AFFINE_WARP_HPP

#include<ramen/image/generic/samplers.hpp>

namespace ramen
{
namespace image
{
namespace sse2
{

void affine_warp_bilinear( const Imath::Box2i& src_area, const const_image_view_t& src,
		     const Imath::Box2i& dst_area, const image_view_t& dst,
		     const matrix3_t& xform, const matrix3_t& inv_xform);

void affine_warp_catrom( const Imath::Box2i& src_area, const const_image_view_t& src,
		     const Imath::Box2i& dst_area, const image_view_t& dst,
		     const matrix3_t& xform, const matrix3_t& inv_xform);

// non black border modes
void affine_warp_bilinear_tile( const Imath::Box2i& src_area, const const_image_view_t& src,
		     const Imath::Box2i& dst_area, const image_view_t& dst,
		     const matrix3_t& xform, const matrix3_t& inv_xform);

void affine_warp_bilinear_mirror( const Imath::Box2i& src_area, const const_image_view_t& src,
		     const Imath::Box2i& dst_area, const image_view_t& dst,
		     const matrix3_t& xform, const matrix3_t& inv_xform);

} // namespace
} // namespace
} // namespace

#endif

