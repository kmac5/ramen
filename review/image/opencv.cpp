// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/image/opencv.hpp>

#include<adobe/algorithm/clamp.hpp>

#include<ramen/image/color.hpp>

namespace ramen
{
namespace image
{

void rgba_image_to_cvmat_8u( const const_image_view_t& src, cv::Mat& dst)
{
	if( dst.type() != CV_8U || dst.cols != src.width() || dst.rows != src.height())
		dst = cv::Mat( src.height(), src.width(), CV_8U);
	
	for( int j = 0; j < src.height(); ++j)
	{
		const_image_view_t::x_iterator src_it( src.row_begin( j));
		unsigned char *dst_it = dst.ptr<unsigned char>( j);
		
		for( int i = 0; i < src.width(); ++i)
		{
			*dst_it++ = adobe::clamp( luminance( *src_it), 0.0f, 1.0f) * 255.0f;
			++src_it;
		}
	}
}

void channel_to_cvmat_8u( const const_channel_view_t& src, cv::Mat& dst)
{
	if( dst.type() != CV_8U || dst.cols != src.width() || dst.rows != src.height())
		dst = cv::Mat( src.height(), src.width(), CV_8U);
	
	for( int j = 0; j < src.height(); ++j)
	{
		const_channel_view_t::x_iterator src_it( src.row_begin( j));
		unsigned char *dst_it = dst.ptr<unsigned char>( j);
		
		for( int i = 0; i < src.width(); ++i)
		{
			*dst_it++ = (*src_it)[0];
			++src_it;
		}
	}	
}

void alpha_channel_to_cvmat_8u( const const_image_view_t& src, cv::Mat& dst)
{
	if( dst.type() != CV_8U || dst.cols != src.width() || dst.rows != src.height())
		dst = cv::Mat( src.height(), src.width(), CV_8U);
	
	for( int j = 0; j < src.height(); ++j)
	{
		const_image_view_t::x_iterator src_it( src.row_begin( j));
		unsigned char *dst_it = dst.ptr<unsigned char>( j);
		
		for( int i = 0; i < src.width(); ++i)
		{
			float a = boost::gil::get_color( *src_it, boost::gil::alpha_t());
			*dst_it++ = adobe::clamp( a, 0.0f, 1.0f) * 255.0f;
			++src_it;
		}
	}
}

void rgba_image_to_cvmat_32fc3( const const_image_view_t& src, cv::Mat& dst)
{
	if( dst.type() != CV_32FC3 || dst.cols != src.width() || dst.rows != src.height())
		dst = cv::Mat( src.height(), src.width(), CV_32FC3);
	
	for( int j = 0; j < src.height(); ++j)
	{
		const_image_view_t::x_iterator src_it( src.row_begin( j));
		float *dst_it = dst.ptr<float>( j);

		for( int i = 0; i < src.width(); ++i)
		{
			*dst_it++ = boost::gil::get_color( *src_it, boost::gil::blue_t());
			*dst_it++ = boost::gil::get_color( *src_it, boost::gil::green_t());
			*dst_it++ = boost::gil::get_color( *src_it, boost::gil::red_t());
			++src_it;
		}
	}
}

void cvmat_32fc3_to_rgba_image( const cv::Mat& src, const image_view_t& dst)
{
	assert( src.type() == CV_32FC3);
	assert( src.cols == dst.width());
	assert( src.rows == dst.height());

	for( int j = 0; j < dst.height(); ++j)
	{
		const float *src_it = src.ptr<float>( j);
		image_view_t::x_iterator dst_it( dst.row_begin( j));

		for( int i = 0; i < dst.width(); ++i)
		{
			boost::gil::get_color( *dst_it, boost::gil::blue_t()) = *src_it++;
			boost::gil::get_color( *dst_it, boost::gil::green_t()) = *src_it++;
			boost::gil::get_color( *dst_it, boost::gil::red_t()) = *src_it++;
			++dst_it;
		}
	}	
}

void invert_cvmat_8u( cv::Mat& dst)
{
	for( int j = 0; j < dst.rows; ++j)
	{
		unsigned char *ptr = dst.ptr<unsigned char>( j);
		for( int i = 0; i < dst.cols; ++i)
		{
			unsigned char v = *ptr;
			*ptr++ = 255 - v;
		}
	}
}

void rgba_subimage_to_cvmat_8u( const const_image_view_t& src, cv::Mat& dst, int xoffset, int yoffset)
{
	assert( dst.type() == CV_8U);
	assert( dst.cols >= src.width() + xoffset);
	assert( dst.rows >= src.height() + yoffset);
	
	for( int j = 0; j < src.height(); ++j)
	{
		const_image_view_t::x_iterator src_it( src.row_begin( j));
		unsigned char *dst_it = dst.ptr<unsigned char>( j + yoffset);
		dst_it += xoffset;
		
		for( int i = 0; i < src.width(); ++i)
		{
			*dst_it++ = adobe::clamp( luminance( *src_it), 0.0f, 1.0f) * 255.0f;
			++src_it;
		}
	}
}

void alpha_subimage_to_cvmat_8u( const const_image_view_t& src, cv::Mat& dst, int xoffset, int yoffset)
{
	assert( dst.type() == CV_8U);
	assert( dst.cols >= src.width() + xoffset);
	assert( dst.rows >= src.height() + yoffset);
	
	for( int j = 0; j < src.height(); ++j)
	{
		const_image_view_t::x_iterator src_it( src.row_begin( j));
		unsigned char *dst_it = dst.ptr<unsigned char>( j + yoffset);
		dst_it += xoffset;
		
		for( int i = 0; i < src.width(); ++i)
		{
			float a = boost::gil::get_color( *src_it, boost::gil::alpha_t());
			*dst_it++ = adobe::clamp( a, 0.0f, 1.0f) * 255.0f;
			++src_it;
		}
	}
}

} // image
} // ramen
