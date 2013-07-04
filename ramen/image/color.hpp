// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_COLOR_HPP
#define RAMEN_IMAGE_COLOR_HPP

#include<algorithm>

#include<OpenEXR/ImathColor.h>

#include<ramen/algorithm/clamp.hpp>

#include<ramen/image/typedefs.hpp>

namespace ramen
{
namespace image
{

// I'm not sure if this belongs here
inline pixel_t premultiply_pixel( const pixel_t& p)
{
    float a = boost::gil::get_color( p, boost::gil::alpha_t());

    return pixel_t( boost::gil::get_color( p, boost::gil::red_t())  * a,
                    boost::gil::get_color( p, boost::gil::green_t())* a,
                    boost::gil::get_color( p, boost::gil::blue_t()) * a,
                    a);
}

inline pixel_t unpremultiply_pixel( const pixel_t& p)
{
    float a = boost::gil::get_color( p, boost::gil::alpha_t());

    if( a != 0.0f)
    {
        return pixel_t( boost::gil::get_color( p, boost::gil::red_t())  / a,
                        boost::gil::get_color( p, boost::gil::green_t())/ a,
                        boost::gil::get_color( p, boost::gil::blue_t()) / a,
                        a);
    }
    else
        return pixel_t( 0.0f, 0.0f, 0.0f, 0.0f);
}

inline pixel_t clamp_negative_values( const pixel_t& p)
{
    return pixel_t( std::max( (float) boost::gil::get_color( p, boost::gil::red_t())  , 0.0f),
                    std::max( (float) boost::gil::get_color( p, boost::gil::green_t()), 0.0f),
                    std::max( (float) boost::gil::get_color( p, boost::gil::blue_t()) , 0.0f),
                    clamp( (float) boost::gil::get_color( p, boost::gil::alpha_t()), 0.0f, 1.0f));
}

inline float luminance( float r, float g, float b)
{
    return  ( r * 0.212671f) +
            ( g * 0.715160f) +
            ( b * 0.072169f);	
}

inline float luminance( const pixel_t& p)
{
    return  luminance(	boost::gil::get_color( p, boost::gil::red_t()), 
						boost::gil::get_color( p, boost::gil::green_t()),
						boost::gil::get_color( p, boost::gil::blue_t()));
}

inline image::pixel_t lerp_pixels( const pixel_t& p, const pixel_t& q, float t)
{
	float it = 1.0f - t;
    return pixel_t( boost::gil::get_color( p, boost::gil::red_t())   * t + boost::gil::get_color( q, boost::gil::red_t())   * it,
					boost::gil::get_color( p, boost::gil::green_t()) * t + boost::gil::get_color( q, boost::gil::green_t()) * it,
					boost::gil::get_color( p, boost::gil::blue_t())  * t + boost::gil::get_color( q, boost::gil::blue_t())  * it,
					boost::gil::get_color( p, boost::gil::alpha_t()) * t + boost::gil::get_color( q, boost::gil::alpha_t()) * it);
}

inline float shadow_blend_function( float x)
{
    if( x < 0.0f)
        return 1.0f;

    if( x > 0.09f)
        return 0.0f;

    float xx = x / 0.09f;
    return ( 2 * xx * xx * xx) - ( 3 * xx * xx) + 1;
}

inline float highlights_blend_function( float x)
{
    if( x < 0.5f)
        return 0.0f;

    if( x > 1.0f)
        return 1.0f;

    float xx = (x - 0.5f)/ 0.5f;
    return -( 2.0f * xx * xx * xx) + ( 3.0f * xx * xx);
}

inline float midtones_blend_function( float x)
{
    return 1.0f - shadow_blend_function(x) - highlights_blend_function(x);
}

// function object versions of color blend functions
struct shadow_blend_fun
{
    template<class T>
    T operator()( T x) const { return shadow_blend_function( x);}
};

struct highlights_blend_fun
{
    template<class T>
    T operator()( T x) const { return highlights_blend_function( x);}
};

struct midtones_blend_fun
{
    template<class T>
    T operator()( T x) const { return midtones_blend_function( x);}
};

// rgb to hsv and back
pixel_t rgb_to_hsv( const pixel_t& src);
Imath::Color4f rgb_to_hsv( const Imath::Color4f& src);
Imath::Color3f rgb_to_hsv( const Imath::Color3f& src);

pixel_t hsv_to_rgb( const pixel_t& src);
Imath::Color4f hsv_to_rgb( const Imath::Color4f& src);
Imath::Color3f hsv_to_rgb( const Imath::Color3f& src);

} // namespace
} // namespace

#endif
