// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_IMAGE_DENAN_HPP
#define RAMEN_IMAGE_DENAN_HPP

#include<ramen/image/typedefs.hpp>

namespace ramen
{
namespace image
{

void denan( const const_image_view_t& src, const image_view_t& dst, float replacement = 0.0f);

} // namespace
} // namespace

#endif
