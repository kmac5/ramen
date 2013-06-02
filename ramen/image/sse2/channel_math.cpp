// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/generic/math.hpp>

#include<emmintrin.h>

#include<tbb/blocked_range.h>
#include<tbb/parallel_for.h>

#include<ramen/assert.hpp>

namespace ramen
{
namespace image
{
namespace sse2
{
namespace
{

struct multiplies_scalar
{
	multiplies_scalar( float s) : s_( s)
	{
		ssss_ = _mm_set_ps( s_, s_, s_, s_);
	}
	
	__m128 operator()( __m128 a) const	{ return _mm_mul_ps( a, ssss_);}
	float operator()( float a) const	{ return a * s_;}
	
private:

	float s_;
	__m128 ssss_;
};
	
template<class ConstGrayView, class GrayView, class Fun>
struct unary_gray_fn
{
	unary_gray_fn( const ConstGrayView& src, const GrayView& dst, Fun f) : src_(src), dst_(dst), f_( f)
	{
		RAMEN_ASSERT( src_.dimensions() == dst_.dimensions());
	}
		
	void operator()( const tbb::blocked_range<std::size_t>& r) const
	{
		for( std::size_t y = r.begin(); y != r.end(); ++y)
		{
			typename ConstGrayView::x_iterator src_it( src_.row_begin( y));
			typename GrayView::y_iterator dst_it( dst_.col_begin( y));
			int i = 0;
			int e = src_.width();

			// SSE2 path
			if( src_.width() >= 4)
			{
				for( i = 0; i < e; i += 4)
				{
					__m128 a  = _mm_set_ps( src_it[0][0], src_it[1][0], src_it[2][0], src_it[3][0]);
					a = f_( a);
	
					float *p = reinterpret_cast<float*>( &a);
					dst_it[0][0] = p[0];
					dst_it[1][0] = p[1];
					dst_it[2][0] = p[2];
					dst_it[3][0] = p[3];
	
					src_it += 4;
					dst_it += 4;
				}
					
				i -= 3;
			}
				
			// extra row pixels
			for( ; i < src_.width(); ++i)
			{
				dst_it[0][0] = f_( src_it[0][0]);
				++src_it;
				++dst_it;
			}
		}
	}
	
private:
	
	ConstGrayView src_;
	GrayView dst_;
	Fun f_;
};

struct plus
{
	__m128 operator()( __m128 a, __m128 b) const	{ return _mm_add_ps( a, b);}
	float operator()( float a, float b) const		{ return a + b;}
};

struct minus
{
	__m128 operator()( __m128 a, __m128 b) const	{ return _mm_sub_ps( a, b);}
	float operator()( float a, float b) const		{ return a - b;}
};

struct multiplies
{
	__m128 operator()( __m128 a, __m128 b) const	{ return _mm_mul_ps( a, b);}
	float operator()( float a, float b) const		{ return a * b;}
};

template<class ConstGrayView, class GrayView, class Fun>
struct binary_gray_fn
{
	binary_gray_fn( const ConstGrayView& src1, const ConstGrayView& src2, 
				 const GrayView& dst, Fun f) : src1_(src1), src2_( src2), dst_(dst), f_( f)
	{
		RAMEN_ASSERT( src1_.dimensions() == src2_.dimensions());
		RAMEN_ASSERT( src1_.dimensions() == dst_.dimensions());
	}
	
	void operator()( const tbb::blocked_range<std::size_t>& r) const
	{
		for( std::size_t y = r.begin(); y != r.end(); ++y)
		{
			typename ConstGrayView::x_iterator src1_it( src1_.row_begin( y));
			typename ConstGrayView::x_iterator src2_it( src2_.row_begin( y));
			typename GrayView::y_iterator dst_it( dst_.col_begin( y));
			int i = 0;
			int e = src1_.width();
			
			// SSE2 path
			if( src1_.width() >= 4)
			{
				for( i = 0; i < e; i += 4)
				{
					__m128 a  = _mm_set_ps( src1_it[0][0], src1_it[1][0], src1_it[2][0], src1_it[3][0]);
					__m128 b  = _mm_set_ps( src2_it[0][0], src2_it[1][0], src2_it[2][0], src2_it[3][0]);
					b = f_( a, b);

					float *p = reinterpret_cast<float*>( &b);
					dst_it[0][0] = p[0];
					dst_it[1][0] = p[1];
					dst_it[2][0] = p[2];
					dst_it[3][0] = p[3];

					src1_it += 4;
					src2_it += 4;
					dst_it += 4;
				}
				
				i -= 3;
			}
			
			// extra row pixels
			for( ; i < e; ++i)
			{
				dst_it[0][0] = f_( src1_it[0][0], src2_it[0][0]);
				++src1_it;
				++src2_it;
				++dst_it;
			}
		}
	}
	
private:

	ConstGrayView src1_, src2_;
	GrayView dst_;
	Fun f_;
};

} // namespace


// gray images
void add_gray_images( const const_gray_image_view_t& a, const const_gray_image_view_t& b, const gray_image_view_t& result)
{	
    tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, a.height()),
					   binary_gray_fn<const_gray_image_view_t, gray_image_view_t, plus>( a, b, result, plus()),
					   tbb::auto_partitioner());	
}

void mul_gray_image_scalar( const const_gray_image_view_t& a, float f, const gray_image_view_t& result)
{
    tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, a.height()),
					   unary_gray_fn<const_gray_image_view_t, gray_image_view_t, multiplies_scalar>( a, result, multiplies_scalar( f)),
					   tbb::auto_partitioner());
}

// channels

} // namespace
} // namespace
} // namespace
