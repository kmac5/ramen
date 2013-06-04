// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_SSE2_RESIZE_HPP
#define RAMEN_IMAGE_SSE2_RESIZE_HPP

#include<ramen/image/typedefs.hpp>

#include<OpenEXR/ImathBox.h>

namespace ramen
{
namespace image
{
namespace sse2
{

void resize_bilinear( const const_image_view_t& src, const image_view_t& dst);

void resize_lanczos3( const image::const_image_view_t& src, const Imath::Box2i& src_defined, const Imath::Box2i& src_area,
					   const image::image_view_t& tmp, const Imath::Box2i& tmp_area,
					   const image::image_view_t& dst, const Imath::Box2i& dst_area,
					  const Imath::V2i& center, const Imath::V2f& scale);

void resize_mitchell( const image::const_image_view_t& src, const Imath::Box2i& src_defined, const Imath::Box2i& src_area,
					   const image::image_view_t& tmp, const Imath::Box2i& tmp_area,
					   const image::image_view_t& dst, const Imath::Box2i& dst_area,
					  const Imath::V2i& center, const Imath::V2f& scale);

void resize_catrom( const image::const_image_view_t& src, const Imath::Box2i& src_defined, const Imath::Box2i& src_area,
					   const image::image_view_t& tmp, const Imath::Box2i& tmp_area,
					   const image::image_view_t& dst, const Imath::Box2i& dst_area,
					  const Imath::V2i& center, const Imath::V2f& scale);

void resize_half( const image::const_image_view_t& src, const Imath::Box2i& src_area,
					const image::image_view_t& dst, const Imath::Box2i& dst_area);

} // namespace
} // namespace
} // namespace

#endif

