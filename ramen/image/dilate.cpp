// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/image/dilate.hpp>

#include<algorithm>
#include<limits>

#include<tbb/blocked_range.h>
#include<tbb/parallel_for.h>

#include<ramen/assert.hpp>

#include<ramen/algorithm/clamp.hpp>

using namespace boost::gil;

namespace ramen
{
namespace image
{
namespace
{

struct dilate_pixel_fun
{
	dilate_pixel_fun()					{ value_ = -std::numeric_limits<float>::max();}
	void operator()( float v)			{ value_ = std::max( value_, v);}
	void operator()( float v, float w)	{ value_ = std::max( value_, v * w);}
	float value() const					{ return value_;}

private:
	
	float value_;
};

struct erode_pixel_fun
{
	erode_pixel_fun()					{ value_ = -std::numeric_limits<float>::max();}
	void operator()( float v)			{ value_ = std::max( value_, 1.0f - v);}
	void operator()( float v, float w)	{ value_ = std::max( value_, ( 1.0f - v) * w);}
	float value() const					{ return 1.0f - value_;}

private:
	
	float value_;
};

template<class ConstGrayView, class GrayView, class PixelFun>
struct dilate_erode_gray_fn
{
    dilate_erode_gray_fn( const ConstGrayView& src, const GrayView& dst, float radius) : src_( src), dst_( dst), radius_( radius) {}

    void operator()( const tbb::blocked_range<std::size_t>& r) const
    {
		int radius = std::floor( (double) radius_);
		float fradius = radius_ - radius;
		
		for( std::size_t y = r.begin(); y != r.end(); ++y)
		{
			typename ConstGrayView::x_iterator src_it( src_.row_begin( y));
			typename GrayView::y_iterator dst_it( dst_.col_begin( y));
			
		    for( int x = 0; x < src_.width(); ++x)
		    {
				PixelFun f;

                int indx = clamp( x -( radius + 1), 0, (int) src_.width() - 1);
				f( src_it[indx][0], fradius);
				
				for ( int i = -radius; i <= radius; i++ )
				{
                    indx = clamp( x + i, 0, (int) src_.width() - 1);
					f( src_it[indx][0]);
				}

                indx = clamp( x + radius + 1, 0, (int) src_.width() - 1);
				f( src_it[indx][0], fradius);
				
				dst_it[x][0] = f.value();
			}
		}
    }

    const ConstGrayView& src_;
    const GrayView& dst_;
    float radius_;
};

template<class ConstGrayView, class TmpView, class GrayView>
void do_dilate_channel( const ConstGrayView& src, const TmpView& tmp, const GrayView& dst, float hradius, float vradius)
{
	RAMEN_ASSERT( src.dimensions() == dst.dimensions());
	RAMEN_ASSERT( tmp.width() >= src.height());
	RAMEN_ASSERT( tmp.height() >= src.width());
	
	if( hradius > 0.0f)
	{
	    tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, src.height()),
						   dilate_erode_gray_fn<ConstGrayView, TmpView, dilate_pixel_fun>( src, tmp, hradius),							
							tbb::auto_partitioner());
	}
	else
	{
	    tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, src.height()),
							dilate_erode_gray_fn<ConstGrayView, TmpView, erode_pixel_fun>( src, tmp, -hradius),
							tbb::auto_partitioner());
	}

	if( vradius > 0.0f)
	{
	    tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, tmp.height()),
							dilate_erode_gray_fn<TmpView, GrayView, dilate_pixel_fun>( tmp, dst, vradius),
							tbb::auto_partitioner());
	}
	else
	{
	    tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, tmp.height()),
							dilate_erode_gray_fn<TmpView, GrayView, erode_pixel_fun>( tmp, dst, -vradius),
							tbb::auto_partitioner());		
	}
}

} // unnamed

void dilate_channel( const const_channel_view_t& src, const channel_view_t& dst, float hradius, float vradius)
{
    if( hradius == 0 && vradius == 0)
    {
		boost::gil::copy_pixels( src, dst);
		return;
    }

    boost::gil::gray32f_image_t buffer( src.height(), src.width());
	
	do_dilate_channel( src, boost::gil::view( buffer), dst, hradius, vradius);
}

void dilate_channel( const const_channel_view_t& src, const boost::gil::gray32f_view_t& tmp, 
					 const channel_view_t& dst, float hradius, float vradius)
{
    if( hradius == 0 && vradius == 0)
    {
		boost::gil::copy_pixels( src, dst);
		return;
    }
	
	do_dilate_channel( src, tmp, dst, hradius, vradius);
}

void dilate( const boost::gil::gray32fc_view_t& src, const boost::gil::gray32f_view_t& dst, float hradius, float vradius)
{
    if( hradius == 0 && vradius == 0)
    {
		boost::gil::copy_pixels( src, dst);
		return;
    }

    boost::gil::gray32f_image_t buffer( src.height(), src.width());

	do_dilate_channel( src, boost::gil::view( buffer), dst, hradius, vradius);
}

void dilate( const boost::gil::gray32fc_view_t& src, const boost::gil::gray32f_view_t& tmp,
			 const boost::gil::gray32f_view_t& dst, float hradius, float vradius)
{
    if( hradius == 0 && vradius == 0)
    {
		boost::gil::copy_pixels( src, dst);
		return;
    }

	do_dilate_channel( src, tmp, dst, hradius, vradius);
}

} // namespace
} // namespace
