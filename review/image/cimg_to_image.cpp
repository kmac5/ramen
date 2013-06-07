// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/image/cimg_to_image.hpp>

#include<ramen/assert.hpp>

namespace ramen
{
namespace image
{

void copy_rgb_image_to_cimg( const const_image_view_t& src, cimg_library::CImg<float>& dst)
{
    if( dst.width() != src.width() || dst.height() != src.height() || dst.depth() != 1 || dst.spectrum() != 3)
	    dst.assign( src.width(), src.height(), 1, 3);

    for( int y=0;y<src.height();++y)
    {
		const_image_view_t::x_iterator src_it = src.row_begin( y);
	
		for( int x=0;x<src.width();++x)
		{
			dst( x, y, 0, 0) = boost::gil::get_color( *src_it, boost::gil::red_t());
			dst( x, y, 0, 1) = boost::gil::get_color( *src_it, boost::gil::green_t());
			dst( x, y, 0, 2) = boost::gil::get_color( *src_it, boost::gil::blue_t());
			++src_it;
		}
    }
}

void copy_cimg_to_rgb_image( const cimg_library::CImg<float>& src, const image_view_t& dst)
{
    RAMEN_ASSERT( src.width() == dst.width() && src.height() == dst.height() && src.spectrum() >= 3);

    for( int y=0;y<dst.height();++y)
    {
		image_view_t::x_iterator dst_it = dst.row_begin( y);
	
		for( int x=0;x<dst.width();++x)
		{
			boost::gil::get_color( *dst_it, boost::gil::red_t())    = src( x, y, 0, 0);
			boost::gil::get_color( *dst_it, boost::gil::green_t())  = src( x, y, 0, 1);
			boost::gil::get_color( *dst_it, boost::gil::blue_t())   = src( x, y, 0, 2);
			++dst_it;
		}
    }
}

void copy_rgba_image_to_cimg( const const_image_view_t& src, cimg_library::CImg<float>& dst)
{
    if( dst.width() != src.width() || dst.height() != src.height() || dst.depth() != 1 || dst.spectrum() != 4)
	    dst.assign( src.width(), src.height(), 1, 4);

    for( int y=0;y<src.height();++y)
    {
		const_image_view_t::x_iterator src_it = src.row_begin( y);

		for( int x=0;x<src.width();++x)
		{
		    dst( x, y, 0, 0) = boost::gil::get_color( *src_it, boost::gil::red_t());
		    dst( x, y, 0, 1) = boost::gil::get_color( *src_it, boost::gil::green_t());
		    dst( x, y, 0, 2) = boost::gil::get_color( *src_it, boost::gil::blue_t());
		    dst( x, y, 0, 3) = boost::gil::get_color( *src_it, boost::gil::alpha_t());
		    ++src_it;
		}
    }
}

void copy_cimg_to_rgba_image( const cimg_library::CImg<float>& src, const image_view_t& dst)
{
    RAMEN_ASSERT( src.width() == dst.width() && src.height() == dst.height() && src.spectrum() >= 4);

    for( int y=0;y<dst.height();++y)
    {
		image_view_t::x_iterator dst_it = dst.row_begin( y);

		for( int x=0;x<dst.width();++x)
		{
		    boost::gil::get_color( *dst_it, boost::gil::red_t())    = src( x, y, 0, 0);
		    boost::gil::get_color( *dst_it, boost::gil::green_t())  = src( x, y, 0, 1);
		    boost::gil::get_color( *dst_it, boost::gil::blue_t())   = src( x, y, 0, 2);
		    boost::gil::get_color( *dst_it, boost::gil::alpha_t())  = src( x, y, 0, 3);
		    ++dst_it;
		}
    }
}

void copy_alpha_image_to_cimg( const const_image_view_t& src, cimg_library::CImg<float>& dst)
{
    if( dst.width() != src.width() || dst.height() != src.height() || dst.depth() != 1 || dst.spectrum() != 1)
	    dst.assign( src.width(), src.height(), 1, 1);

    for( int y=0;y<src.height();++y)
    {
		const_image_view_t::x_iterator src_it = src.row_begin( y);

		for( int x=0;x<src.width();++x)
		{
		    dst( x, y) = boost::gil::get_color( *src_it, boost::gil::alpha_t());
		    ++src_it;
		}
    }
}

void copy_cimg_to_alpha_image( const cimg_library::CImg<float>& src, const image_view_t& dst)
{
    RAMEN_ASSERT( src.width() == dst.width() && src.height() == dst.height());

    for( int y=0;y<dst.height();++y)
    {
		image_view_t::x_iterator dst_it = dst.row_begin( y);
	
		for( int x=0;x<dst.width();++x)
		{
		    boost::gil::get_color( *dst_it, boost::gil::alpha_t())  = src( x, y);
		    ++dst_it;
		}
    }
}

} // namespace
} // namespace
