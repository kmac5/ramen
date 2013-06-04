// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_SSE2_WARP_HPP
#define RAMEN_IMAGE_SSE2_WARP_HPP

#include<ramen/image/typedefs.hpp>

#include<OpenEXR/ImathBox.h>

#include<ramen/image/warp.hpp>

namespace ramen
{
namespace image
{
namespace sse2
{

void warp_bilinear( const Imath::Box2i& src_area, const const_image_view_t& src,
				const Imath::Box2i& dst_area, const image_view_t& dst,
				const warp_function_t& wfun, bool uv_derivs, bool sequential);

void warp_bilinear_tile( const Imath::Box2i& src_area, const const_image_view_t& src,
				const Imath::Box2i& dst_area, const image_view_t& dst,
				const warp_function_t& wfun, bool uv_derivs, bool sequential);

void warp_bilinear_mirror( const Imath::Box2i& src_area, const const_image_view_t& src,
				const Imath::Box2i& dst_area, const image_view_t& dst,
				const warp_function_t& wfun, bool uv_derivs, bool sequential);

void warp_bicubic( const Imath::Box2i& src_area, const const_image_view_t& src,
					const Imath::Box2i& dst_area, const image_view_t& dst,
					const warp_function_t& wfun, bool uv_derivs, bool sequential);

} // namespace
} // namespace
} // namespace

#endif
