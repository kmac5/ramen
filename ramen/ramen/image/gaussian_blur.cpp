// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/image/gaussian_blur.hpp>

#include<ramen/assert.hpp>

#include<ramen/image/processing.hpp>

namespace ramen
{
namespace image
{

void gaussian_blur_rgba( const const_image_view_t& src, const image_view_t& dst, float stddevx, float stddevy)
{
    if( stddevx == 0 && stddevy == 0)
    {
		boost::gil::copy_pixels( src, dst);
		return;
    }

	image_t buffer( src.height(), src.width());
	gaussian_blur_rgba_( src, boost::gil::view( buffer), dst, stddevx, stddevy);
}

void gaussian_blur_rgba( const const_image_view_t& src, const image_view_t& tmp, const image_view_t& dst, float stddevx, float stddevy)
{
	RAMEN_ASSERT( src.width() == tmp.height());
	RAMEN_ASSERT( src.height() == tmp.width());
	
    if( stddevx == 0 && stddevy == 0)
    {
		boost::gil::copy_pixels( src, dst);
		return;
    }

	gaussian_blur_rgba_( src, tmp, dst, stddevx, stddevy);	
}

void gaussian_blur_rgb( const const_rgb_image_view_t& src, const rgb_image_view_t& dst, float stddevx, float stddevy)
{
    if( stddevx == 0 && stddevy == 0)
    {
		boost::gil::copy_pixels( src, dst);
		return;
    }

	rgb_image_t buffer( src.height(), src.width());
	gaussian_blur_rgb_( src, boost::gil::view( buffer), dst, stddevx, stddevy);
}

void gaussian_blur_rgb( const const_rgb_image_view_t& src, const rgb_image_view_t& tmp, const rgb_image_view_t& dst, float stddevx, float stddevy)
{
	RAMEN_ASSERT( src.width() == tmp.height());
	RAMEN_ASSERT( src.height() == tmp.width());
	
    if( stddevx == 0 && stddevy == 0)
    {
		boost::gil::copy_pixels( src, dst);
		return;
    }

	gaussian_blur_rgb_( src, tmp, dst, stddevx, stddevy);	
}

void gaussian_blur_channel( const const_channel_view_t& src, const channel_view_t& dst, float stddevx, float stddevy)
{
    if( stddevx == 0 && stddevy == 0)
    {
		boost::gil::copy_pixels( src, dst);
		return;
    }

	boost::gil::gray32f_image_t buffer( src.height(), src.width());
	gaussian_blur_channel_( src, boost::gil::view( buffer), dst, stddevx, stddevy);	
}

void gaussian_blur_channel( const const_channel_view_t& src, const boost::gil::gray32f_view_t& tmp, 
					   const channel_view_t& dst, float stddevx, float stddevy)
{
	RAMEN_ASSERT( src.width() == tmp.height());
	RAMEN_ASSERT( src.height() == tmp.width());

    if( stddevx == 0 && stddevy == 0)
    {
		boost::gil::copy_pixels( src, dst);
		return;
    }

	gaussian_blur_channel_( src, tmp, dst, stddevx, stddevy);
}

void gaussian_blur_gray( const boost::gil::gray32fc_view_t& src, const boost::gil::gray32f_view_t& dst, float stddevx, float stddevy)
{
    if( stddevx == 0 && stddevy == 0)
    {
		boost::gil::copy_pixels( src, dst);
		return;
    }
	
	boost::gil::gray32f_image_t buffer( src.height(), src.width());
	gaussian_blur_gray_( src, boost::gil::view( buffer), dst, stddevx, stddevy);
}

void gaussian_blur_gray( const boost::gil::gray32fc_view_t& src, const boost::gil::gray32f_view_t& tmp, 
					const boost::gil::gray32f_view_t& dst, float stddevx, float stddevy)
{
	RAMEN_ASSERT( src.width() == tmp.height());
	RAMEN_ASSERT( src.height() == tmp.width());

    if( stddevx == 0 && stddevy == 0)
    {
		boost::gil::copy_pixels( src, dst);
		return;
    }
	
	gaussian_blur_gray_( src, tmp, dst, stddevx, stddevy);	
}

} // namespace
} // namespace
