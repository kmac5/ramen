// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_BOX_BLUR_MASK_HPP
#define RAMEN_IMAGE_BOX_BLUR_MASK_HPP

#include<vector>
#include<algorithm>

#include<tbb/blocked_range.h>
#include<tbb/parallel_for.h>

#include<OpenEXR/ImathVec.h>
#include<OpenEXR/ImathFun.h>

#include<ramen/image/typedefs.hpp>
#include<ramen/assert.hpp>

namespace ramen
{
namespace image
{
namespace detail
{

template<class ConstMaskView>
struct box_blur_radius_fun
{

	box_blur_radius_fun( const ConstMaskView& mask, const Imath::V2i& mask_pos) : mask_( mask)
	{
		mask_pos_ = mask_pos;
	}
	
	float operator()( int x, int y) const
	{
		int xx = x - mask_pos_.x;
		int yy = y - mask_pos_.y;
		
		if( xx < 0 || xx >= mask_.width())
			return 0.0f;
		
		if( yy < 0 || yy >= mask_.height())
			return 0.0f;
		
		return mask_( xx, yy)[0];
	}
	
private:
	
	const ConstMaskView& mask_;
	Imath::V2i mask_pos_;
};
	
template<class Fun>
struct transpose_fun
{
	transpose_fun( Fun f) : f_( f) {}
	
	float operator()( int x, int y) const { return f_( y, x);}
	
private:
	
	Fun f_;
};

template<class ConstGrayView, class GrayView, class MaskFun>
struct box_blur_mask_fun
{
public:

	box_blur_mask_fun( const ConstGrayView& src, float min_radius, float max_radius,
					   MaskFun mf, const GrayView& dst) : src_( src), mfun_( mf), dst_( dst)
	{
		RAMEN_ASSERT( min_radius >= 0 && max_radius >= 0);
		min_radius_ = min_radius;
		max_radius_ = max_radius;
		
		pad_ = Imath::Math<float>::ceil( std::max( min_radius_, max_radius_)) + 1;
		table_.resize( src_.width() + 2 * pad_, 0);
	}
		
	void operator()( const tbb::blocked_range<std::size_t>& r) const
	{
		for( int y = r.begin(); y < r.end(); ++y)
		{
			typename ConstGrayView::x_iterator src_it( src_.row_begin( y));
			typename ConstGrayView::x_iterator src_end( src_.row_end( y));
			make_table( src_it, src_end);

			typename GrayView::y_iterator dst_it( dst_.col_begin( y));
			
			for( int x = 0, xe = src_.width(); x < xe; ++x)
			{
				float alpha = mfun_( x, y);
				alpha = Imath::clamp( alpha, 0.0f, 1.0f);
				float radius = Imath::lerp( min_radius_, max_radius_, alpha);
				float area = 2.0f * radius + 1.0f;
				float val = mean( x, radius);
				dst_it[x][0] = val / area;
			}
		}
	}
		
private:
		
	template<class Iter>
	void make_table( Iter start, Iter end) const
	{
		for( int i = 0; i < pad_; ++i)
			table_[i] =  ( *start) [0] * ( i + 1);

		float val;
		
		for( int i = 0, ie = src_.width(); i < ie; ++i)
		{
			int index = i + pad_;
			val = (*start)[0];
			table_[index] = table_[index - 1] + val;
			++start;
		}
				
		for( int i = 0; i < pad_; ++i)
		{
			int index = i + pad_ + src_.width();
			table_[ index] = table_[ index - 1] + val;
		}
	}
	
	float sample_table( float x) const
	{
		int xi = Imath::Math<float>::floor( x);
		float xf = x - xi;
		return Imath::lerp( table_[xi], table_[xi+1], xf);
	}
		
	float mean( int p, float radius) const
	{
		RAMEN_ASSERT( p >= 0 && p < src_.width());
		float a = sample_table( p + pad_ - radius - 1);
		float b = sample_table( p + pad_ + radius);
		return ( b - a);
	}
	
	const ConstGrayView& src_;
	
	MaskFun mfun_;
	float min_radius_, max_radius_;

	// summed area table
	mutable std::vector<float> table_;
	int pad_;
	const GrayView& dst_;
};
	
} // namespace

template<class ConstGrayView, class ConstMaskView, class TmpView, class GrayView>
void box_blur_mask( const ConstGrayView& src, const ConstMaskView& mask, const Imath::V2i& mask_pos, 
					const Imath::V2f& min_radius, const Imath::V2f& max_radius, int iters, 
					const TmpView& tmp, const GrayView& dst)
{
	typedef detail::box_blur_radius_fun<ConstMaskView>	mask_fun_t;
	typedef detail::transpose_fun<mask_fun_t>			transposed_mask_fun_t;

	mask_fun_t mf( mask, mask_pos);
	transposed_mask_fun_t tmf( mf);
		
	// first iter
	{
		// horizontal pass
		detail::box_blur_mask_fun<ConstGrayView, TmpView, mask_fun_t> fun_h( src, min_radius.x, max_radius.x, mf, tmp);
		tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, src.height()), fun_h, tbb::auto_partitioner());
	
		// vertical pass
		detail::box_blur_mask_fun<TmpView, GrayView, transposed_mask_fun_t> fun_v( tmp, min_radius.y, max_radius.y, tmf, dst);
		tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, tmp.height()), fun_v, tbb::auto_partitioner());
	}
	
	for( int i = 1; i < iters; ++i)
	{
		// horizontal pass
		detail::box_blur_mask_fun<GrayView, TmpView, mask_fun_t> fun_h( dst, min_radius.x, max_radius.x, mf, tmp);
		tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, dst.height()), fun_h, tbb::auto_partitioner());
	
		// vertical pass
		detail::box_blur_mask_fun<TmpView, GrayView, transposed_mask_fun_t> fun_v( tmp, min_radius.y, max_radius.y, tmf, dst);
		tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, tmp.height()), fun_v, tbb::auto_partitioner());
	}
}

} // namespace
} // namespace

#endif
