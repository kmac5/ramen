// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/sse2/samplers.hpp>

#include<emmintrin.h>

#include<cmath>

#include<OpenEXR/ImathMath.h>
#include<OpenEXR/ImathFun.h>

#include<ramen/image/filters.hpp>

namespace ramen
{
namespace image
{
namespace sse2
{
namespace
{

__m128 bicubic_sample( int x, int y, const image::const_image_view_t& src, const Imath::Box2i& src_area, float xweights[4], float yweights[4])
{
	int width = src.width();
	int height = src.height();
	
    __m128 result = _mm_setzero_ps();

    for( int j = -1; j <= 2; ++j)
    {
        image::const_image_view_t::x_iterator src_it( src.row_begin( clamp( y - src_area.min.y + j, 0, height - 1)));
		int sx = x - src_area.min.x - 1;

        __m128 yw = _mm_set_ps( yweights[j+1], yweights[j+1], yweights[j+1], yweights[j+1]);
        __m128 row = _mm_setzero_ps();

        for( int i = 0; i < 4; ++i)
        {
            __m128 pix = _mm_load_ps( reinterpret_cast<const float*>( &(src_it[ clamp( sx + i, 0, width - 1)])));
			__m128 xw = _mm_set1_ps( xweights[i]);
            row = _mm_add_ps( row, _mm_mul_ps( pix, xw));
        }

        result = _mm_add_ps( result, _mm_mul_ps( row, yw));
    }
	
	return result;
}

__m128 lanczos3_sample( int x, int y, const image::const_image_view_t& src, const Imath::Box2i& src_area, float xweights[10], float yweights[10])
{
	int width = src.width();
	int height = src.height();

    __m128 result = _mm_setzero_ps();

    for( int j = -5; j <= 4; ++j)
    {
        image::const_image_view_t::x_iterator src_it( src.row_begin( clamp( y - src_area.min.y + j, 0, height - 1)));
		int sx = x - src_area.min.x - 1;

        __m128 yw = _mm_set_ps( yweights[j+5], yweights[j+1], yweights[j+5], yweights[j+5]);
        __m128 row = _mm_setzero_ps();

        for( int i = 0; i < 10; ++i)
        {
            __m128 pix = _mm_load_ps( reinterpret_cast<const float*>( &(src_it[ clamp( sx + i, 0, width - 1)])));
			__m128 xw = _mm_set1_ps( xweights[i]);
            row = _mm_add_ps( row, _mm_mul_ps( pix, xw));
        }

        result = _mm_add_ps( result, _mm_mul_ps( row, yw));
    }

	return result;
}

} // unnamed

/**********************************************************************************/

bilinear_sampler_t::bilinear_sampler_t( const const_image_view_t& src) : generic::sampler_t( src) {}
bilinear_sampler_t::bilinear_sampler_t( const Imath::Box2i& src_area, const const_image_view_t& src) : generic::sampler_t( src_area, src) {}

pixel_t bilinear_sampler_t::operator()( const vector2_t& p) const
{
	int y = math::fast_float_floor( p.y);
	
	if( y < src_area_.min.y || y > src_area_.max.y)
        return pixel_t( 0, 0, 0, 0);

	int x = math::fast_float_floor( p.x);
	
	if( x < src_area_.min.x || x > src_area_.max.x)
        return pixel_t( 0, 0, 0, 0);

	float t = p.x - x;
	float s = p.y - y;
	
    __m128 tx = _mm_set1_ps( t);
    __m128 ix = _mm_set1_ps( 1.0f - t);

	const_image_view_t::x_iterator top_it( src_.row_begin( y - src_area_.min.y));
	const_image_view_t::x_iterator bot_it;

	if( y != src_area_.max.y)
		bot_it = src_.row_begin( y + 1 - src_area_.min.y);
	else
		bot_it = top_it;

	__m128 a = _mm_load_ps( reinterpret_cast<const float*>( &( top_it[ x - src_area_.min.x])));
	__m128 b;
	
	if( x != src_area_.max.x)
	    b = _mm_load_ps( reinterpret_cast<const float*>( &( top_it[ x + 1- src_area_.min.x])));
	else
	    b = a;

    __m128 top = _mm_add_ps( _mm_mul_ps( a, ix), _mm_mul_ps( b, tx));
	
	a = _mm_load_ps( reinterpret_cast<const float*>( &( bot_it[ x - src_area_.min.x])));
	
	if( x != src_area_.max.x)
	    b = _mm_load_ps( reinterpret_cast<const float*>( &( bot_it[ x + 1- src_area_.min.x])));
	else
	    b = a;
    
	__m128 bot = _mm_add_ps( _mm_mul_ps( a, ix), _mm_mul_ps( b, tx));

    tx = _mm_set1_ps( s);
    ix = _mm_set1_ps( 1.0f - s);
    a = _mm_add_ps( _mm_mul_ps( top, ix), _mm_mul_ps( bot, tx));
	
    float *q = reinterpret_cast<float*>( &a);
    return pixel_t( q[0], q[1], q[2], q[3]);
}

/**********************************************************************************/

catrom_sampler_t::catrom_sampler_t( const const_image_view_t& src) : generic::sampler_t( src) {}
catrom_sampler_t::catrom_sampler_t( const Imath::Box2i& src_area, const const_image_view_t& src) : generic::sampler_t( src_area, src) {}

pixel_t catrom_sampler_t::operator()( const vector2_t& p) const
{
	int y = math::fast_float_floor( p.y);
	
	if( y < src_area_.min.y || y > src_area_.max.y)
        return pixel_t( 0, 0, 0, 0);

	int x = math::fast_float_floor( p.x);
	
	if( x < src_area_.min.x || x > src_area_.max.x)
        return pixel_t( 0, 0, 0, 0);

    float fractx = p.x - x;
    float fracty = p.y - y;
	
    float xweights[4], yweights[4];

    weights( fractx, xweights);
    weights( fracty, yweights);
	
	__m128 result = bicubic_sample( x, y, src_, src_area_, xweights, yweights);
    float *q = reinterpret_cast<float*>( &result);
    return pixel_t( q[0], q[1], q[2], Imath::clamp( q[3], 0.0f, 1.0f));
}

void catrom_sampler_t::weights( float t, float w[4]) const
{
    float t2 = t * t, t3 = t2 * t;
    w[0] = ( -t3 + t2 + t2 - t) * 0.5f;
    w[1] = ( 3.0f * t3 - 5.0f * t2 + 2.0f) * 0.5f;
    w[2] = ( -3.0f * t3 + 4.0f * t2 + t) * 0.5f;
    w[3] = ( t3 - t2) * 0.5f;
}

/**********************************************************************************/

lanczos3_sampler_t::lanczos3_sampler_t( const const_image_view_t& src) : generic::sampler_t( src) {}
lanczos3_sampler_t::lanczos3_sampler_t( const Imath::Box2i& src_area, const const_image_view_t& src) : generic::sampler_t( src_area, src) {}

pixel_t lanczos3_sampler_t::operator()( const vector2_t& p) const
{
	int y = math::fast_float_floor( p.y);

	if( y < src_area_.min.y || y > src_area_.max.y)
        return pixel_t( 0, 0, 0, 0);

	int x = math::fast_float_floor( p.x);

	if( x < src_area_.min.x || x > src_area_.max.x)
        return pixel_t( 0, 0, 0, 0);

    float fractx = p.x - x;
    float fracty = p.y - y;

	float xweights[10], yweights[10];

    weights( x, fractx, xweights);
    weights( y, fracty, yweights);

	__m128 result = lanczos3_sample( x, y, src_, src_area_, xweights, yweights);
    float *q = reinterpret_cast<float*>( &result);
    return pixel_t( q[0], q[1], q[2], Imath::clamp( q[3], 0.0f, 1.0f));

}

void lanczos3_sampler_t::weights( int c, float t, float w[10]) const
{
	lanczos3_filter_t filter;

	for( int dx = -5, i = 0; dx <= 4; ++dx, ++i)
		w[i] = filter( dx - t);
}

} // namespace
} // namespace
} // namespace
