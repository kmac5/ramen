// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_GENERIC_RGBTOGRAY_HPP
#define	RAMEN_IMAGE_GENERIC_RGBTOGRAY_HPP

#include<ramen/gil/extension/algorithm/tbb/tbb_transform.hpp>

#include<ramen/image/color.hpp>

namespace ramen
{
namespace image
{
namespace generic
{
namespace detail
{

struct rgb_to_gray
{
    image::gray_pixel_t operator()( const pixel_t& src) const
    {
	return image::gray_pixel_t( luminance( src));
    }
};

} // namespace

void convert_rgb_to_gray( const const_image_view_t& src, const gray_image_view_t& dst)
{
    boost::gil::tbb_transform_pixels( src, dst, detail::rgb_to_gray());
}

} // namespace
} // namespace
} // namespace

#endif

