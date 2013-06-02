// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_SSE2_CHANNEL_MATH_HPP
#define	RAMEN_IMAGE_SSE2_CHANNEL_MATH_HPP

#include<ramen/image/typedefs.hpp>

namespace ramen
{
namespace image
{
namespace sse2
{

void add_gray_images( const const_gray_image_view_t& a, const const_gray_image_view_t& b, const gray_image_view_t& result);
void mul_gray_image_scalar( const const_gray_image_view_t& a, float f, const gray_image_view_t& result);
	
} // namespace
} // namespace
} // namespace

#endif
