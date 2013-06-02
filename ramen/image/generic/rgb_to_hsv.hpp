// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_GENERIC_RGB_TO_HSV_HPP
#define	RAMEN_IMAGE_GENERIC_RGB_TO_HSV_HPP

#include<ramen/gil/extension/algorithm/tbb/tbb_transform.hpp>

#include<ramen/assert.hpp>

#include<ramen/image/color.hpp>

namespace ramen
{
namespace image
{
namespace generic
{

void convert_rgb_to_hsv( const const_image_view_t& src, const image_view_t& dst)
{
    RAMEN_ASSERT( src.width() == dst.width());
    RAMEN_ASSERT( src.height() == dst.height());
	
	pixel_t (*rgb_to_hsv_fun)( const pixel_t&) = rgb_to_hsv;
    boost::gil::tbb_transform_pixels( src, dst, rgb_to_hsv_fun);
}

void convert_hsv_to_rgb( const const_image_view_t& src, const image_view_t& dst)
{
    RAMEN_ASSERT( src.width() == dst.width());
    RAMEN_ASSERT( src.height() == dst.height());

	pixel_t (*hsv_to_rgb_fun)( const pixel_t&) = hsv_to_rgb;
    boost::gil::tbb_transform_pixels( src, dst, hsv_to_rgb_fun);
}

} // namespace
} // namespace
} // namespace

#endif
