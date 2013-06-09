// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/generic/layer_modes.hpp>

#include<ramen/algorithm/clamp.hpp>

using namespace boost::gil;

namespace ramen
{
namespace image
{
namespace generic
{
namespace
{

inline image::pixel_t lerp_pixel( const image::pixel_t& a, const image::pixel_t& b, float t)
{
    float t_inv = 1.0f - t;
    return image::pixel_t(  ( get_color( a, red_t())   * t) + ( get_color( b, red_t())   * t_inv),
							( get_color( a, green_t()) * t) + ( get_color( b, green_t()) * t_inv),
							( get_color( a, blue_t())  * t) + ( get_color( b, blue_t())  * t_inv),
							( get_color( a, alpha_t()) * t) + ( get_color( b, alpha_t()) * t_inv));
}

} // unnamed

screen_layer_mode_fun::screen_layer_mode_fun( float opacity) : opacity_( opacity) {}

pixel_t screen_layer_mode_fun::operator()( const pixel_t& back, const pixel_t& front) const
{
    pixel_t result( 1.0f - ((1.0f - get_color( front, red_t()))   * (1.0f - get_color( back, red_t()))),
					1.0f - ((1.0f - get_color( front, green_t())) * (1.0f - get_color( back, green_t()))),
					1.0f - ((1.0f - get_color( front, blue_t()))  * (1.0f - get_color( back, blue_t()))),
                    clamp( get_color( front, alpha_t()) + get_color( back, alpha_t()), 0.0f, 1.0f));

    get_color( result, red_t())     = std::max( get_color( result, red_t())  , get_color( back, red_t()));
    get_color( result, green_t())   = std::max( get_color( result, green_t()), get_color( back, green_t()));
    get_color( result, blue_t())    = std::max( get_color( result, blue_t()) , get_color( back, blue_t()));
    return lerp_pixel( result, back, opacity_);
}

overlay_layer_mode_fun::overlay_layer_mode_fun( float opacity) : opacity_( opacity) {}

pixel_t overlay_layer_mode_fun::operator()( const pixel_t& back, const pixel_t& front) const
{
    float rb = get_color( back, red_t());
    float gb = get_color( back, green_t());
    float bb = get_color( back, blue_t());

    float rf = get_color( front, red_t());
    float gf = get_color( front, green_t());
    float bf = get_color( front, blue_t());

    rf = overlay( rf, rb);
    gf = overlay( gf, gb);
    bf = overlay( bf, bb);

    pixel_t result( rf, gf, bf, clamp( get_color( front, alpha_t()) + get_color( back, alpha_t()), 0.0f, 1.0f));
    return lerp_pixel( result, back, opacity_);
}

zero_overlay_layer_mode_fun::zero_overlay_layer_mode_fun( float opacity) : opacity_( opacity) {}

pixel_t zero_overlay_layer_mode_fun::operator()( const pixel_t& p) const
{
    float rb = get_color( p, red_t());
    float gb = get_color( p, green_t());
    float bb = get_color( p, blue_t());

    float rf = overlay( 0, rb);
    float gf = overlay( 0, gb);
    float bf = overlay( 0, bb);

    pixel_t result( rf, gf, bf, get_color( p, alpha_t()));
    return lerp_pixel( result, p, opacity_);
}

diff_layer_mode_fun::diff_layer_mode_fun( float opacity) : opacity_( opacity) {}

pixel_t diff_layer_mode_fun::operator()( const pixel_t& back, const pixel_t& front) const
{
    pixel_t result( abs( get_color( back, red_t())   - get_color( front, red_t())),
					abs( get_color( back, green_t()) - get_color( front, green_t())),
					abs( get_color( back, blue_t())  - get_color( front, blue_t())),
                    abs( clamp( get_color( back, alpha_t()) - get_color( front, alpha_t()), 0.0f, 1.0f)));

    return lerp_pixel( result, back, opacity_);
}

} // namespace
} // namespace
} // namespace
