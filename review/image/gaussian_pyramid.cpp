// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/image/gaussian_pyramid.hpp>

#include<cmath>

#include<adobe/algorithm/clamp.hpp>

#include<ramen/assert.hpp>

#include<ramen/image/gaussian_blur.hpp>

namespace ramen
{
namespace image
{

gray_gaussian_pyramid_t::gray_gaussian_pyramid_t( int width, int height, float ratio, int min_width) : gray_pyramid_t( width, height, ratio, min_width)
{
}
	
void gray_gaussian_pyramid_t::build()
{
	float sigma = ( 1.0f / ratio_ - 1.0f);
	
	buffer_t tmp( const_view().width(), const_view().height(), 1);
	buffer_t tmp2( tmp.height(), tmp.width(), 1);

	for( int i = 1; i < num_levels_; ++i)
	{
		int w = const_view( i-1).width();
		int h = const_view( i-1).height();
			
		image::gray_image_view_t tmp_view( boost::gil::subimage_view( tmp.gray_view(), 0, 0, w, h));
		gaussian_blur_gray( const_view( i-1), boost::gil::subimage_view( tmp2.gray_view(), 0, 0, h, w), tmp_view, sigma, sigma);
		scale_view( tmp_view, view( i));
	}
}

/**********************************************************************************************/
	
rgb_gaussian_pyramid_t::rgb_gaussian_pyramid_t( int width, int height, float ratio, int min_width) : rgb_pyramid_t( width, height, ratio, min_width)
{
}
	
void rgb_gaussian_pyramid_t::build()
{
	float sigma = ( 1.0f / ratio_ - 1.0f);
	
	buffer_t tmp( const_view().width(), const_view().height(), 3);
	buffer_t tmp2( tmp.height(), tmp.width(), 3);
		
	for( int i = 1; i < num_levels_; ++i)
	{
		int w = const_view( i-1).width();
		int h = const_view( i-1).height();
			
		image::rgb_image_view_t tmp_view( boost::gil::subimage_view( tmp.rgb_view(), 0, 0, w, h));
		gaussian_blur_rgb( const_view( i-1), boost::gil::subimage_view( tmp2.rgb_view(), 0, 0, h, w), tmp_view, sigma, sigma);
		scale_view( tmp_view, view( i));
	}
}

/**********************************************************************************************/
	
rgba_gaussian_pyramid_t::rgba_gaussian_pyramid_t( int width, int height, float ratio, int min_width) : rgba_pyramid_t( width, height, ratio, min_width)
{
}

void rgba_gaussian_pyramid_t::build()
{
	float sigma = ( 1.0f / ratio_ - 1.0f);

	buffer_t tmp( const_view().width(), const_view().height(), 4);
	buffer_t tmp2( tmp.height(), tmp.width(), 4);
	
	for( int i = 1; i < num_levels_; ++i)
	{
		int w = const_view( i-1).width();
		int h = const_view( i-1).height();
		
		image::image_view_t tmp_view( boost::gil::subimage_view( tmp.rgba_view(), 0, 0, w, h));
		gaussian_blur_rgba( const_view( i-1), boost::gil::subimage_view( tmp2.rgba_view(), 0, 0, h, w), tmp_view, sigma, sigma);
		scale_view( tmp_view, view( i));
	}
}

} // namespace
} // namespace
