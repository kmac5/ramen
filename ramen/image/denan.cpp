// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/denan.hpp>

#include<boost/math/special_functions/fpclassify.hpp>

#include<ramen/gil/extension/algorithm/tbb/tbb_transform.hpp>

namespace ramen
{
namespace image
{
namespace
{

struct replace_nan_fun
{
	replace_nan_fun( float new_val) : new_val_( new_val) {}
		
	image::pixel_t operator()( const image::pixel_t& p) const
	{
		float r = boost::gil::get_color( p, boost::gil::red_t());
		float g = boost::gil::get_color( p, boost::gil::green_t());
		float b = boost::gil::get_color( p, boost::gil::blue_t());
		float a = boost::gil::get_color( p, boost::gil::alpha_t());
			
		if( boost::math::isnan( r)) r = new_val_;
		if( boost::math::isnan( g)) g = new_val_;
		if( boost::math::isnan( b)) b = new_val_;
		if( boost::math::isnan( a)) a = new_val_;
		return image::pixel_t( r, g, b, a);
	}
	
private:

	float new_val_;
};
	
} // detail

void denan( const const_image_view_t& src, const image_view_t& dst, float replacement)
{
	replace_nan_fun f( replacement);
	boost::gil::tbb_transform_pixels( src, dst, f);
}

} // image
} // ramen
