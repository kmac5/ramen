// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/generic/apply_gamma_exp_log.hpp>

#include<algorithm>

#include<ramen/gil/extension/algorithm/tbb/tbb_transform.hpp>

#include<ramen/half/halfExpLog.h>

namespace ramen
{
namespace image
{
namespace generic
{

namespace
{

struct apply_gamma_fun
{
    apply_gamma_fun( float f) : g_( f) {}

    pixel_t operator()( const pixel_t& src) const
    {
		pixel_t result;
		half h;

		h = std::max( (float) boost::gil::get_color( src, boost::gil::red_t()), 0.0f);
		h = pow_h( h, g_);
		boost::gil::get_color( result, boost::gil::red_t()) = h;

		h = std::max( (float) boost::gil::get_color( src, boost::gil::green_t()), 0.0f);
		h = pow_h( h, g_);
		boost::gil::get_color( result, boost::gil::green_t()) = h;

		h = std::max( (float) boost::gil::get_color( src, boost::gil::blue_t()), 0.0f);
		h = pow_h( h, g_);
		boost::gil::get_color( result, boost::gil::blue_t()) = h;

		boost::gil::get_color( result, boost::gil::alpha_t()) = boost::gil::get_color( src, boost::gil::alpha_t());
		return result;
    }

private:

    float g_;
};

struct apply_log_fun
{
    apply_log_fun() {}

    pixel_t operator()( const pixel_t& src) const
    {
		float r = std::max( (float) boost::gil::get_color( src, boost::gil::red_t()), (float) HALF_MIN);
		float g = std::max( (float) boost::gil::get_color( src, boost::gil::green_t()), (float) HALF_MIN);
		float b = std::max( (float) boost::gil::get_color( src, boost::gil::blue_t()), (float) HALF_MIN);

		r = log10_h( r);
		g = log10_h( g);
		b = log10_h( b);
		
		return pixel_t( r, g, b, boost::gil::get_color( src, boost::gil::alpha_t()));
    }
};

struct apply_pow10_fun
{
    apply_pow10_fun() {}

    pixel_t operator()( const pixel_t& src) const
    {
		float r = boost::gil::get_color( src, boost::gil::red_t());
		float g = boost::gil::get_color( src, boost::gil::green_t());
		float b = boost::gil::get_color( src, boost::gil::blue_t());

		r = pow10_h( r);
		g = pow10_h( g);
		b = pow10_h( b);
		
		return pixel_t( r, g, b, boost::gil::get_color( src, boost::gil::alpha_t()));
    }
};

} // unnamed

void apply_gamma( const const_image_view_t& src, const image_view_t& dst, float gamma)
{
    boost::gil::tbb_transform_pixels( src, dst, apply_gamma_fun( gamma));
}

void apply_log( const const_image_view_t& src, const image_view_t& dst)
{
    boost::gil::tbb_transform_pixels( src, dst, apply_log_fun());
}

void apply_pow10( const const_image_view_t& src, const image_view_t& dst)
{
    boost::gil::tbb_transform_pixels( src, dst, apply_pow10_fun());
}

} // namespace
} // namespace
} // namespace
