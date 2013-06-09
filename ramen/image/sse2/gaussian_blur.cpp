// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/sse2/gaussian_blur.hpp>

#include<algorithm>
#include<cmath>
#include<vector>

#include<emmintrin.h>

#include<ramen/algorithm/clamp.hpp>

#include<tbb/blocked_range.h>
#include<tbb/parallel_for.h>

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

struct gauss_blur_rgba_fn
{
    gauss_blur_rgba_fn( const const_image_view_t& src, const image_view_t& dst, __m128 *kernel, int size) : src_(src), dst_(dst)
	{
		kernel_ = kernel;
		k_size_ = size;
	}

    void operator()( const tbb::blocked_range<std::size_t>& r) const
    {
		int xoff = ( k_size_ - 1) / 2;

		for( std::size_t y = r.begin(); y != r.end(); ++y)
		{
			const_image_view_t::x_iterator src_it( src_.row_begin( y));
			image_view_t::y_iterator dst_it( dst_.col_begin( y));
			
			for( int x = 0, e = src_.width(); x < e; ++x)
			{
				__m128 *k_it = kernel_;
				__m128 accum = _mm_setzero_ps();
				
				for( int i = -xoff; i <= xoff; ++i)
				{
                    int indx = clamp( x + i, 0, (int) src_.width() - 1);
					__m128 p = _mm_load_ps( reinterpret_cast<const float*>( &( src_it[indx])));
					accum = _mm_add_ps( accum, _mm_mul_ps( p, *k_it++));
				}
				
				_mm_store_ps( reinterpret_cast<float*>( &( dst_it[x])), accum);				
			}
		}
    }

    const const_image_view_t& src_;
    const image_view_t& dst_;
	__m128 *kernel_;
	int k_size_;
};

void make_gauss_kernel( __m128 *kernel, int size, float dev)
{
	if( dev == 0)
	{
		kernel[0] = _mm_setzero_ps();
		kernel[1] = _mm_set1_ps( 1.0f);
		kernel[2] = _mm_setzero_ps();
		return;
	}
	
	int radius = size / 2;
	float sum = 0;
	
	for (int i = 0; i < size; i++)
	{
		float diff = ( i - radius)/ dev;
		float value = std::exp(-diff * diff / 2);
		kernel[i] = _mm_set1_ps( value);		
		sum += value;
	}

	__m128 norm = _mm_set1_ps( 1.0f / sum);
	
	for (int i = 0; i < size; i++)
		kernel[i] = _mm_mul_ps( kernel[i], norm);
}

} // unnamed

void gaussian_blur_rgba( const const_image_view_t& src, const image_view_t& tmp, const image_view_t& dst, float std_devx, float std_devy)
{
	// create kernel here
	int sizex = (int)( std_devx * 6 + 1) | 1;
	if( sizex == 1) sizex = 3;
	
	int sizey = (int)( std_devy * 6 + 1) | 1;
	if( sizey == 1) sizey = 3;
	
	__m128 *kernel = new __m128[ std::max( sizex, sizey)];

	make_gauss_kernel( kernel, sizex, std_devx);
    tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, src.height()),
			gauss_blur_rgba_fn( src, tmp, kernel, sizex), tbb::auto_partitioner());

	make_gauss_kernel( kernel, sizey, std_devy);
    tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, tmp.height()),
			gauss_blur_rgba_fn( tmp, dst, kernel, sizey), tbb::auto_partitioner());

	delete[] kernel;
}

} // namespace
} // namespace
} // namespace
