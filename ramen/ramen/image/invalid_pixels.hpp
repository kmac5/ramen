// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_IMAGE_INVALID_PIXELS_HPP
#define	RAMEN_IMAGE_INVALID_PIXELS_HPP

#include<ramen/image/typedefs.hpp>

namespace ramen
{
namespace image
{

void count_invalid_pixels( const image_view_t& img, std::size_t& nan_pixels, std::size_t& neg_pixels);

} // namespace
} // namespace

#endif
