// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/color.hpp>

#include<cmath>

namespace ramen
{
namespace image
{

// rgb to hsv and back
pixel_t rgb_to_hsv( const pixel_t& src)
{
	Imath::Color3f c( boost::gil::get_color( src, boost::gil::red_t()),
						boost::gil::get_color( src, boost::gil::green_t()),
						boost::gil::get_color( src, boost::gil::blue_t()));

	c = rgb_to_hsv( c);

    pixel_t result;
    boost::gil::get_color( result, boost::gil::red_t())   = c.x;
    boost::gil::get_color( result, boost::gil::green_t()) = c.y;
    boost::gil::get_color( result, boost::gil::blue_t())  = c.z;
    boost::gil::get_color( result, boost::gil::alpha_t()) = boost::gil::get_color( src, boost::gil::alpha_t());
    return result;
}

Imath::Color4f rgb_to_hsv( const Imath::Color4f& src)
{
	Imath::Color3f c( src.r, src.g, src.b);
	c = rgb_to_hsv( c);
	return Imath::Color4f( c.x, c.y, c.z, src.a);
}

Imath::Color3f rgb_to_hsv( const Imath::Color3f& src)
{
	float r = src.x;
	float g = src.y;
	float b = src.z;
	
    // normalize RGB to 0 .. 1
    if( r < 0) r = 0;
    if( g < 0) g = 0;
    if( b < 0) b = 0;

    float scale = std::max( r, std::max( g, b));

    if( scale < 1.0f)
        scale = 1.0f;

    r /= scale;
    g /= scale;
    b /= scale;

    float maxv = std::max( r, std::max( g, b));
    float minv = std::min( r, std::min( g, b));

    float h = 0, s = 0, v = maxv * scale;

    if( maxv != minv)
    {
        const float f = ( r == minv) ? ( g - b) : (( g == minv) ? ( b - r) : ( r - g)),
                    i = ( r == minv) ? 3.0f : (( g == minv) ? 5.0f : 1.0f);

        h = ( i - f / ( maxv - minv));

        if( h >= 6.0f)
            h -= 6.0f;

        h /= 6.0f;
        s = ( maxv - minv) / maxv;
    }
	
	return Imath::Color3f( h, s, v);
}

pixel_t hsv_to_rgb( const pixel_t& src)
{
	Imath::Color3f c( boost::gil::get_color( src, boost::gil::red_t()),
						boost::gil::get_color( src, boost::gil::green_t()),
						boost::gil::get_color( src, boost::gil::blue_t()));

	c = hsv_to_rgb( c);

    pixel_t result;
    boost::gil::get_color( result, boost::gil::red_t())   = c.x;
    boost::gil::get_color( result, boost::gil::green_t()) = c.y;
    boost::gil::get_color( result, boost::gil::blue_t())  = c.z;
    boost::gil::get_color( result, boost::gil::alpha_t()) = boost::gil::get_color( src, boost::gil::alpha_t());
    return result;
}

Imath::Color4f hsv_to_rgb( const Imath::Color4f& src)
{
	Imath::Color3f c( src.r, src.g, src.b);
	c = hsv_to_rgb( c);
	return Imath::Color4f( c.x, c.y, c.z, src.a);
}

Imath::Color3f hsv_to_rgb( const Imath::Color3f& src)
{
    float h = src.x;
    float s = src.y;
    float v = src.z;

    float scale = std::max( v, 1.0f);
    v /= scale;

    float r = 0, g = 0, b = 0;

    if( h == 0 && s == 0)
        r = g = b = v;
    else
    {
        h *= 6.0f;
        const int i = (int) std::floor( (double) h);
        const float f = ( i & 1) ? ( h - i) : ( 1.0f - h + i),
                    m = v * ( 1.0f - s),
                    n = v * ( 1.0f - s * f);
        switch(i)
        {
        case 6:
        case 0: r = v; g = n; b = m; break;
        case 1: r = n; g = v; b = m; break;
        case 2: r = m; g = v; b = n; break;
        case 3: r = m; g = n; b = v; break;
        case 4: r = n; g = m; b = v; break;
        case 5: r = v; g = m; b = n; break;
        }
    }

    r *= scale;
    g *= scale;
    b *= scale;
	return Imath::Color3f( r, g, b);
}

} // namespace
} // namespace
