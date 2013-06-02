// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/image/invalid_pixels.hpp>

#include<cmath>

namespace ramen
{
namespace image
{

namespace detail
{

struct count_invalid_pixels_fun
{
    count_invalid_pixels_fun() : nans( 0), negs( 0)
	{
	}

    void operator()( const image::pixel_t& p)
    {
		float r = boost::gil::get_color( p, boost::gil::red_t());
		float g = boost::gil::get_color( p, boost::gil::green_t());
		float b = boost::gil::get_color( p, boost::gil::blue_t());
		float a = boost::gil::get_color( p, boost::gil::alpha_t());
		
		if( isnan( r) || isnan( g) || isnan( b) || isnan( a))
		{
			nans++;
			return;
		}
		
		if( r < 0 || g < 0 || b < 0 || a < 0)
		{
			negs++;
			return;
		}
    }

	std::size_t nans;
	std::size_t negs;
};

} // detail

void count_invalid_pixels( const image_view_t& img, std::size_t& nan_pixels, std::size_t& neg_pixels)
{
	detail::count_invalid_pixels_fun f;
	f = boost::gil::for_each_pixel( img, f);

	nan_pixels = f.nans;
	neg_pixels = f.negs;
}

} // namespace
} // namespace
