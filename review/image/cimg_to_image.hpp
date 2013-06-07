// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_IMAGE_CIMG_TO_IMAGE_HPP
#define RAMEN_IMAGE_CIMG_TO_IMAGE_HPP

#include<ramen/config.hpp>

#include<ramen/image/typedefs.hpp>

#include<ramen/image/cimg.hpp>

namespace ramen
{
namespace image
{

void RAMEN_API copy_rgb_image_to_cimg( const const_image_view_t& src, cimg_library::CImg<float>& dst);
void RAMEN_API copy_cimg_to_rgb_image( const cimg_library::CImg<float>& src, const image_view_t& dst);

void RAMEN_API copy_rgba_image_to_cimg( const const_image_view_t& src, cimg_library::CImg<float>& dst);
void RAMEN_API copy_cimg_to_rgba_image( const cimg_library::CImg<float>& src, const image_view_t& dst);

void RAMEN_API copy_alpha_image_to_cimg( const const_image_view_t& src, cimg_library::CImg<float>& dst);
void RAMEN_API copy_cimg_to_alpha_image( const cimg_library::CImg<float>& src, const image_view_t& dst);

} // namespace
} // namespace

#endif
