// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/generic/box_blur_channel.hpp>

#include<algorithm>

#include<tbb/blocked_range.h>
#include<tbb/parallel_for.h>

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

template<class ConstGrayView, class GrayView>
struct box_blur_gray_fn
{
    box_blur_gray_fn( const ConstGrayView& src, const GrayView& dst, float radius) : src_( src), dst_( dst), radius_( radius) {}

    void operator()( const tbb::blocked_range<std::size_t>& r) const
    {
		int radius = std::floor( (double) radius_);
		float fradius = radius_ - radius;
		float inv_fradius = 1.0f - fradius;
		
		float norm = 1.0f / (( 2 * radius + 1) + ( 2 * fradius));

		for( std::size_t y = r.begin(); y != r.end(); ++y)
		{
			typename ConstGrayView::x_iterator src_it( src_.row_begin( y));
			typename GrayView::y_iterator dst_it( dst_.col_begin( y));

            int indx = clamp( -( radius + 1), 0, (int) src_.width() - 1);
			float accum = src_it[indx][0] * fradius;
	
			for ( int i = -radius; i <= radius; i++ )
			{
                indx = clamp( i, 0, (int) src_.width() - 1);
				accum += src_it[indx][0];
			}

            indx = clamp( radius + 1, 0, (int) src_.width() - 1);
			accum += src_it[indx][0] * fradius;

		    for( int x = 0, e = src_.width(); x < e; ++x)
		    {
				float val = accum * norm;
				
				if( val < 0.0001)
					val = 0;
				
				dst_it[x][0] = val;

				int j = std::max( x - radius - 1, 0);
				accum -= src_it[j][0] * fradius;

				j = std::max( x - radius, 0);
				accum -= src_it[j][0] * inv_fradius;

				j = std::min( x + radius + 1, (int) src_.width()-1);
				accum += src_it[j][0] * inv_fradius;

				j = std::min( x + radius + 2, (int) src_.width()-1);
				accum += src_it[j][0] * fradius;
		    }
		}
    }

    const ConstGrayView& src_;
    const GrayView& dst_;
    float radius_;
};

template<class ConstGrayView, class GrayView>
box_blur_gray_fn<ConstGrayView, GrayView> make_box_blur_gray_fn( const ConstGrayView& src, const GrayView& dst, float radius)
{
    return box_blur_gray_fn<ConstGrayView, GrayView>( src, dst, radius);
}

template<class ConstGrayView, class TmpView, class GrayView>
void do_box_blur_channel( const ConstGrayView& src, const TmpView& tmp, const GrayView& dst, float hradius, float vradius, int iters)
{
    // run first iter
    tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, src.height()),
						make_box_blur_gray_fn( src, tmp, hradius), tbb::auto_partitioner());

    tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, tmp.height()),
						make_box_blur_gray_fn( tmp, dst, vradius), tbb::auto_partitioner());

    for( int i = 1; i < iters; ++i)
    {
		tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, dst.height()),
							make_box_blur_gray_fn( dst, tmp, hradius), tbb::auto_partitioner());

		tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, tmp.height()),
							make_box_blur_gray_fn( tmp, dst, vradius), tbb::auto_partitioner());
    }
}

} // unnamed

void box_blur_channel( const const_channel_view_t& src, const boost::gil::gray32f_view_t& tmp, 
					   const channel_view_t& dst, float hradius, float vradius, int iters)
{
	do_box_blur_channel( src, tmp, dst, hradius, vradius, iters);
}

void box_blur_gray( const boost::gil::gray32fc_view_t& src, const boost::gil::gray32f_view_t& tmp, 
					const boost::gil::gray32f_view_t& dst, float hradius, float vradius, int iters)
{
	do_box_blur_channel( src, tmp, dst, hradius, vradius, iters);
}

} // namespace
} // namespace
} // namespace
