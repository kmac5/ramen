// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_IMAGE_GAUSSIAN_BLUR_HPP
#define RAMEN_IMAGE_GAUSSIAN_BLUR_HPP

#include<ramen/image/typedefs.hpp>

namespace ramen
{
namespace image
{
	
void gaussian_blur_rgba( const const_image_view_t& src, const image_view_t& dst, float stddevx, float stddevy);
void gaussian_blur_rgba( const const_image_view_t& src, const image_view_t& tmp, const image_view_t& dst, float stddevx, float stddevy);

void gaussian_blur_rgb( const const_rgb_image_view_t& src, const rgb_image_view_t& dst, float stddevx, float stddevy);
void gaussian_blur_rgb( const const_rgb_image_view_t& src, const rgb_image_view_t& tmp, const rgb_image_view_t& dst, float stddevx, float stddevy);

void gaussian_blur_channel( const const_channel_view_t& src, const channel_view_t& dst, float stddevx, float stddevy);

void gaussian_blur_channel( const const_channel_view_t& src, const boost::gil::gray32f_view_t& tmp, 
					   const channel_view_t& dst, float stddevx, float stddevy);

void gaussian_blur_gray( const boost::gil::gray32fc_view_t& src, const boost::gil::gray32f_view_t& dst, float stddevx, float stddevy);

void gaussian_blur_gray( const boost::gil::gray32fc_view_t& src, const boost::gil::gray32f_view_t& tmp, 
					const boost::gil::gray32f_view_t& dst, float stddevx, float stddevy);

} // namespace
} // namespace

#endif
