// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_IMAGE_OPENCV_HPP
#define RAMEN_IMAGE_OPENCV_HPP

#include<opencv2/core/core.hpp>

#include<ramen/image/typedefs.hpp>

namespace ramen
{
namespace image
{

void rgba_image_to_cvmat_8u( const const_image_view_t& src, cv::Mat& dst);
void channel_to_cvmat_8u( const const_channel_view_t& src, cv::Mat& dst);

void rgba_subimage_to_cvmat_8u( const const_image_view_t& src, cv::Mat& dst, int xoffset, int yoffset);
void alpha_subimage_to_cvmat_8u( const const_image_view_t& src, cv::Mat& dst, int xoffset, int yoffset);

void rgba_image_to_cvmat_32fc3( const const_image_view_t& src, cv::Mat& dst);
void cvmat_32fc3_to_rgba_image( const cv::Mat& src, const image_view_t& dst);
	
void invert_cvmat_8u( cv::Mat& dst);

} // image
} // ramen

#endif

