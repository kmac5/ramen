// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_SSE2_GAUSSIAN_BLUR_HPP
#define RAMEN_IMAGE_SSE2_GAUSSIAN_BLUR_HPP

#include<ramen/image/typedefs.hpp>

namespace ramen
{
namespace image
{
namespace sse2
{

void gaussian_blur_rgba( const const_image_view_t& src, const image_view_t& tmp, 
					const image_view_t& dst, float std_devx, float std_devy);

} // namespace
} // namespace
} // namespace

#endif
