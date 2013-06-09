// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/generic/gaussian_blur_rgb.hpp>

#include<algorithm>
#include<cmath>

#include<ramen/algorithm/clamp.hpp>

#include<tbb/blocked_range.h>
#include<tbb/parallel_for.h>

#include<ramen/image/color.hpp>

using namespace boost::gil;

namespace ramen
{
namespace image
{
namespace generic
{
namespace
{

void make_gauss_kernel( float *kernel, int size, float dev)
{
	if( dev == 0)
	{
		kernel[0] = 0.0f;
		kernel[1] = 1.0f;
		kernel[2] = 0.0f;
		return;
	}
	
	int radius = size / 2;
	float sum = 0;
	
	for (int i = 0; i < size; i++)
	{
		float diff = ( i - radius)/ dev;
		float value = std::exp(-diff * diff / 2);
		kernel[i] = value;
		sum += value;
	}

	float norm = 1.0f / sum;
	
	for (int i = 0; i < size; i++)
		kernel[i] *= norm;
}

struct gaussian_blur_rgb_fn
{
    gaussian_blur_rgb_fn( const const_rgb_image_view_t& src, const rgb_image_view_t& dst, float *kernel, int size) : src_(src), dst_(dst)
	{
		kernel_ = kernel;
		k_size_ = size;
	}

    void operator()( const tbb::blocked_range<std::size_t>& r) const
    {
		int xoff = ( k_size_ - 1) / 2;

		for( std::size_t y = r.begin(); y != r.end(); ++y)
		{
			const_rgb_image_view_t::x_iterator src_it( src_.row_begin( y));
			rgb_image_view_t::y_iterator dst_it( dst_.col_begin( y));

			for( int x = 0, e = src_.width(); x < e; ++x)
			{
				float *k_it = kernel_;
				float r_accum = 0.0f;
				float g_accum = 0.0f;
				float b_accum = 0.0f;
				
				for( int i = -xoff; i <= xoff; ++i)
				{
                    int indx = clamp( x + i, 0, (int) src_.width() - 1);
					
					float r = boost::gil::get_color( src_it[indx], boost::gil::red_t());
					float g = boost::gil::get_color( src_it[indx], boost::gil::green_t());
					float b = boost::gil::get_color( src_it[indx], boost::gil::blue_t());
					
					r_accum += ( r * *k_it);
					g_accum += ( g * *k_it);
					b_accum += ( b * *k_it);
					++k_it;
				}
				
				boost::gil::get_color( *dst_it, boost::gil::red_t()) = r_accum;
				boost::gil::get_color( *dst_it, boost::gil::green_t()) = g_accum;
				boost::gil::get_color( *dst_it, boost::gil::blue_t()) = b_accum;
				++dst_it;
			}
		}
    }

    const const_rgb_image_view_t& src_;
    const rgb_image_view_t& dst_;
	float *kernel_;
	int k_size_;
};

} // unnamed

	
void gaussian_blur_rgb( const const_rgb_image_view_t& src, const rgb_image_view_t& tmp, 
						const rgb_image_view_t& dst, float stddevx, float stddevy)
{
	int sizex = (int)( stddevx * 6 + 1) | 1;
	if( sizex == 1) sizex = 3;
	
	int sizey = (int)( stddevy * 6 + 1) | 1;
	if( sizey == 1) sizey = 3;
	
	float *kernel = new float[ std::max( sizex, sizey)];

	make_gauss_kernel( kernel, sizex, stddevx);
	
    tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, src.height()),
					   gaussian_blur_rgb_fn( src, tmp, kernel, sizex), tbb::auto_partitioner());

	make_gauss_kernel( kernel, sizey, stddevy);
    tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, tmp.height()),
					   gaussian_blur_rgb_fn( tmp, dst, kernel, sizey), tbb::auto_partitioner());

	delete[] kernel;
}

} // namespace
} // namespace
} // namespace
