// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/generic/alpha_layer_modes.hpp>

#include<ramen/algorithm/clamp.hpp>

using namespace boost::gil;

namespace ramen
{
namespace image
{
namespace generic
{

alpha_add_layer_mode_fun::alpha_add_layer_mode_fun( float opacity) : opacity_( opacity) {}

pixel_t alpha_add_layer_mode_fun::operator()( const pixel_t& back, const pixel_t& front) const
{
    return pixel_t( get_color( back, red_t()),
				    get_color( back, green_t()),
				    get_color( back, blue_t()),
                    clamp( get_color( front, alpha_t()) * opacity_ + get_color( back, alpha_t()), 0.0f, 1.0f));
}

alpha_mul_layer_mode_fun::alpha_mul_layer_mode_fun( float opacity) : opacity_( opacity) {}

pixel_t alpha_mul_layer_mode_fun::operator()( const pixel_t& back, const pixel_t& front) const
{
    return pixel_t( get_color( back, red_t()), get_color( back, green_t()), get_color( back, blue_t()),
					( get_color( front, alpha_t()) * opacity_ + 1.0f - opacity_) * get_color( back, alpha_t()));
}

alpha_sub_layer_mode_fun::alpha_sub_layer_mode_fun( float opacity) : opacity_( opacity) {}

pixel_t alpha_sub_layer_mode_fun::operator()( const pixel_t& back, const pixel_t& front) const
{
    return pixel_t( get_color( back, red_t()), get_color( back, green_t()), get_color( back, blue_t()),
                    clamp( get_color( back, alpha_t()) - get_color( front, alpha_t()) * opacity_, 0.0f, 1.0f));
}

alpha_mix_layer_mode_fun::alpha_mix_layer_mode_fun( float opacity) : opacity_( opacity) {}

pixel_t alpha_mix_layer_mode_fun::operator()( const pixel_t& back, const pixel_t& front) const
{
    float a = opacity_;
    float a_inv = 1.0f - a;

    return pixel_t( get_color( back, red_t()), get_color( back, green_t()), get_color( back, blue_t()),
                    clamp( a * get_color( front, alpha_t()) + get_color( back, alpha_t()) * a_inv , 0.0f, 1.0f));
}

alpha_max_layer_mode_fun::alpha_max_layer_mode_fun( float opacity) : opacity_( opacity) {}

pixel_t alpha_max_layer_mode_fun::operator()( const pixel_t& back, const pixel_t& front) const
{
	float bga = get_color( back, alpha_t());
	float a = std::max( (float) get_color( front, alpha_t()) ,bga);
	a = ( a * opacity_) + (( 1.0f - opacity_) * bga);
    return pixel_t( get_color( back, red_t()), get_color( back, green_t()), get_color( back, blue_t()), a);
}

alpha_min_layer_mode_fun::alpha_min_layer_mode_fun( float opacity) : opacity_( opacity) {}

pixel_t alpha_min_layer_mode_fun::operator()( const pixel_t& back, const pixel_t& front) const
{
	float bga = get_color( back, alpha_t());
	float a = std::min( (float) get_color( front, alpha_t()) ,bga);
	a = ( a * opacity_) + (( 1.0f - opacity_) * bga);
    return pixel_t( get_color( back, red_t()), get_color( back, green_t()), get_color( back, blue_t()), a);
}

} // namespace
} // namespace
} // namespace
