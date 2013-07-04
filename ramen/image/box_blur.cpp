// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/box_blur.hpp>

#include<ramen/assert.hpp>

#include<ramen/image/processing.hpp>

namespace ramen
{
namespace image
{

void box_blur_rgba( const const_image_view_t& src, const image_view_t& dst, float hradius, float vradius, int iters)
{
    if( hradius == 0 && vradius == 0)
    {
		boost::gil::copy_pixels( src, dst);
		return;
    }

	image_t buffer( src.height(), src.width());
	box_blur_rgba_( src, boost::gil::view( buffer), dst, hradius, vradius, iters);
}

void box_blur_rgba( const const_image_view_t& src, const image_view_t& tmp, const image_view_t& dst, float hradius, float vradius, int iters)
{
	RAMEN_ASSERT( src.width() == tmp.height());
	RAMEN_ASSERT( src.height() == tmp.width());
	
    if( hradius == 0 && vradius == 0)
    {
		boost::gil::copy_pixels( src, dst);
		return;
    }

	box_blur_rgba_( src, tmp, dst, hradius, vradius, iters);	
}

void box_blur_channel( const const_channel_view_t& src, const channel_view_t& dst, float hradius, float vradius, int iters)
{
    if( hradius == 0 && vradius == 0)
    {
		boost::gil::copy_pixels( src, dst);
		return;
    }

	boost::gil::gray32f_image_t buffer( src.height(), src.width());
	box_blur_channel_( src, boost::gil::view( buffer), dst, hradius, vradius, iters);	
}

void box_blur_channel( const const_channel_view_t& src, const boost::gil::gray32f_view_t& tmp, 
					   const channel_view_t& dst, float hradius, float vradius, int iters)
{
	RAMEN_ASSERT( src.width() == tmp.height());
	RAMEN_ASSERT( src.height() == tmp.width());

    if( hradius == 0 && vradius == 0)
    {
		boost::gil::copy_pixels( src, dst);
		return;
    }

	box_blur_channel_( src, tmp, dst, hradius, vradius, iters);
}

void box_blur_gray( const boost::gil::gray32fc_view_t& src, const boost::gil::gray32f_view_t& dst, float hradius, float vradius, int iters)
{
    if( hradius == 0 && vradius == 0)
    {
		boost::gil::copy_pixels( src, dst);
		return;
    }
	
	boost::gil::gray32f_image_t buffer( src.height(), src.width());
	box_blur_gray_( src, boost::gil::view( buffer), dst, hradius, vradius, iters);
}

void box_blur_gray( const boost::gil::gray32fc_view_t& src, const boost::gil::gray32f_view_t& tmp, 
					const boost::gil::gray32f_view_t& dst, float hradius, float vradius, int iters)
{
	RAMEN_ASSERT( src.width() == tmp.height());
	RAMEN_ASSERT( src.height() == tmp.width());

    if( hradius == 0 && vradius == 0)
    {
		boost::gil::copy_pixels( src, dst);
		return;
    }
	
	box_blur_gray_( src, tmp, dst, hradius, vradius, iters);	
}

} // namespace
} // namespace
