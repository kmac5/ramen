// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/sse2/box_blur.hpp>

#include<algorithm>
#include<cmath>

#include<emmintrin.h>

#include<tbb/blocked_range.h>
#include<tbb/parallel_for.h>

#include<ramen/algorithm/clamp.hpp>
#include<ramen/image/color.hpp>

using namespace boost::gil;

namespace ramen
{
namespace image
{
namespace sse2
{
namespace
{

struct box_blur_rgba_fn
{
    box_blur_rgba_fn( const const_image_view_t& src, const image_view_t& dst, float radius) : src_(src), dst_(dst), radius_(radius) {}

    void operator()( const tbb::blocked_range<std::size_t>& r) const
    {
		int radius = std::floor( ( double) radius_);
		float fradius = radius_ - radius;
        __m128 fractw = _mm_set_ps( fradius, fradius, fradius, fradius);
        __m128 inv_fractw = _mm_set_ps( 1.0f - fradius, 1.0f - fradius, 1.0f - fradius, 1.0f - fradius);

		float norm = 1.0f / ( ( 2 * radius + 1) + ( 2 * fradius));		
        __m128 allw = _mm_set_ps( norm, norm, norm, norm);
		
		__m128 lo_clamp = _mm_set1_ps( 0.0001);
		__m128 zero = _mm_setzero_ps();
		__m128 hi_clamp = _mm_set_ps( 1.0f,
									  std::numeric_limits<float>::max(),
									  std::numeric_limits<float>::max(),
									  std::numeric_limits<float>::max());
		
		for( std::size_t y = r.begin(); y != r.end(); ++y)
		{
			const_image_view_t::x_iterator src_it( src_.row_begin( y));
			image_view_t::y_iterator dst_it( dst_.col_begin( y));

            int indx = clamp( -( radius + 1), 0, (int) src_.width() - 1);
			__m128 p = _mm_load_ps( reinterpret_cast<const float*>( &( src_it[indx])));
			__m128 accum = _mm_mul_ps( p, fractw);
	
			for ( int i = -radius; i <= radius; i++ )
			{
                indx = clamp( i, 0, (int) src_.width() - 1);
				p = _mm_load_ps( reinterpret_cast<const float*>( &( src_it[indx])));
				accum = _mm_add_ps( accum, p);
			}

            indx = clamp( radius + 1, 0, (int) src_.width() - 1);
			p = _mm_load_ps( reinterpret_cast<const float*>( &( src_it[indx])));
			accum = _mm_add_ps( accum, _mm_mul_ps( p, fractw));
			
			for( int x = 0, e = src_.width(); x < e; ++x)
			{
				p = _mm_mul_ps( accum, allw);
				p = _mm_sub_ps( p, lo_clamp);
				p = _mm_max_ps( p, zero);
				p = _mm_min_ps( p, hi_clamp);
				_mm_store_ps( reinterpret_cast<float*>( &( dst_it[x])), p);

				int j = std::max( x - radius - 1, 0);
				p = _mm_load_ps( reinterpret_cast<const float*>( &( src_it[j])));
				accum = _mm_sub_ps( accum, _mm_mul_ps( p, fractw));

				j = std::max( x - radius, 0);
				p = _mm_load_ps( reinterpret_cast<const float*>( &( src_it[j])));
				accum = _mm_sub_ps( accum, _mm_mul_ps( p, inv_fractw));

				j = std::min( x + radius + 1, (int) src_.width()-1);
				p = _mm_load_ps( reinterpret_cast<const float*>( &( src_it[j])));
				accum = _mm_add_ps( accum, _mm_mul_ps( p, inv_fractw));

				j = std::min( x + radius + 2, (int) src_.width()-1);
				p = _mm_load_ps( reinterpret_cast<const float*>( &( src_it[j])));
				accum = _mm_add_ps( accum, _mm_mul_ps( p, fractw));
			}
		}
    }

    const const_image_view_t& src_;
    const image_view_t& dst_;
    float radius_;
};

} // unnamed

void box_blur_rgba( const const_image_view_t& src, const image_view_t& tmp, const image_view_t& dst, float hradius, float vradius, int iters)
{
    // run first iter
    tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, src.height()),
			box_blur_rgba_fn( src, tmp, hradius), tbb::auto_partitioner());

    tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, tmp.height()),
			box_blur_rgba_fn( tmp, dst, vradius), tbb::auto_partitioner());

    for( int i = 1; i < iters; ++i)
    {
		tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, dst.height()),
							box_blur_rgba_fn( dst, tmp, hradius), tbb::auto_partitioner());

		tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, tmp.height()),
							box_blur_rgba_fn( tmp, dst, vradius), tbb::auto_partitioner());	
    }
}

} // namespace
} // namespace
} // namespace
