// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_GENERIC_GAUSSIAN_BLUR_RGB_HPP
#define RAMEN_IMAGE_GENERIC_GAUSSIAN_BLUR_RGB_HPP

#include<ramen/image/typedefs.hpp>

namespace ramen
{
namespace image
{
namespace generic
{
	
void gaussian_blur_rgb( const const_rgb_image_view_t& src, const rgb_image_view_t& tmp, 
						const rgb_image_view_t& dst, float stddevx, float stddevy);
	
} // namespace
} // namespace
} // namespace

#endif
