// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/generic/gaussian_blur_channel.hpp>

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

template<class ConstGrayView, class GrayView>
struct gaussian_blur_gray_fn
{
    gaussian_blur_gray_fn( const ConstGrayView& src, const GrayView& dst, float *kernel, int size) : src_(src), dst_(dst)
	{
		kernel_ = kernel;
		k_size_ = size;
	}

    void operator()( const tbb::blocked_range<std::size_t>& r) const
    {
		int xoff = ( k_size_ - 1) / 2;

		for( std::size_t y = r.begin(); y != r.end(); ++y)
		{
			typename ConstGrayView::x_iterator src_it( src_.row_begin( y));
			typename GrayView::y_iterator dst_it( dst_.col_begin( y));

			for( int x = 0, e = src_.width(); x < e; ++x)
			{
				float *k_it = kernel_;
				float accum = 0.0f;
				
				for( int i = -xoff; i <= xoff; ++i)
				{
                    int indx = clamp( x + i, 0, (int) src_.width() - 1);
					float p = src_it[indx][0];
					accum += ( p * ( *k_it++));
				}
				
				dst_it[x][0] = accum;
			}
		}
    }

    const ConstGrayView& src_;
    const GrayView& dst_;
	float *kernel_;
	int k_size_;
};

template<class ConstGrayView, class GrayView>
gaussian_blur_gray_fn<ConstGrayView, GrayView> make_gaussian_blur_gray_fn( const ConstGrayView& src, const GrayView& dst, 
																		   float *kernel, int size)
{
    return gaussian_blur_gray_fn<ConstGrayView, GrayView>( src, dst, kernel, size);
}

template<class ConstGrayView, class TmpView, class GrayView>
void do_gaussian_blur_gray( const ConstGrayView& src, const TmpView& tmp, const GrayView& dst, float stddevx, float stddevy)
{
	// create kernel here
	int sizex = (int)( stddevx * 6 + 1) | 1;
	if( sizex == 1) sizex = 3;
	
	int sizey = (int)( stddevy * 6 + 1) | 1;
	if( sizey == 1) sizey = 3;
	
	float *kernel = new float[ std::max( sizex, sizey)];

	make_gauss_kernel( kernel, sizex, stddevx);
    tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, src.height()),
						make_gaussian_blur_gray_fn( src, tmp, kernel, sizex), tbb::auto_partitioner());

	make_gauss_kernel( kernel, sizey, stddevy);
    tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, tmp.height()),
						make_gaussian_blur_gray_fn( tmp, dst, kernel, sizey), tbb::auto_partitioner());

	delete[] kernel;
}

} // unnamed

void gaussian_blur_channel( const const_channel_view_t& src, const boost::gil::gray32f_view_t& tmp, 
						   const channel_view_t& dst, float stddevx, float stddevy)
{
	do_gaussian_blur_gray( src, tmp, dst, stddevx, stddevy);
}
	
void gaussian_blur_gray( const boost::gil::gray32fc_view_t& src, const boost::gil::gray32f_view_t& tmp, 
						const boost::gil::gray32f_view_t& dst, float stddevx, float stddevy)
{
	do_gaussian_blur_gray( src, tmp, dst, stddevx, stddevy);
}

} // namespace
} // namespace
} // namespace
