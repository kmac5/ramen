// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/sse2/resize.hpp>

#include<emmintrin.h>

#include<algorithm>

#include<tbb/blocked_range.h>
#include<tbb/parallel_for.h>

#include<ramen/assert.hpp>

#include<ramen/image/generic/resize.hpp>
#include<ramen/image/filters.hpp>

#include<ramen/image/sse2/samplers.hpp>

namespace ramen
{
namespace image
{
namespace sse2
{
namespace
{

float resize_coord( float x, float center, float scale)
{
	return ( ( x - center + 0.5f) / scale - 0.5f) + center;
}

template<class Filter>
struct resize_x_fun
{
	resize_x_fun(	const image::const_image_view_t& src, const Imath::Box2i& src_area,
					const image::image_view_t& dst, const Imath::Box2i& dst_area,
					int xcenter, float scale) : src_( src), dst_( dst)
	{
		scale_ = scale;
		src_area_ = src_area;
		dst_area_ = dst_area;
		xcenter_ = xcenter;
	}

	void operator()( const tbb::blocked_range<int>& r) const
	{
		using namespace boost::gil;

		Filter filter;

		float filter_width = std::max( 1.0f, scale_);
	    int filter_area = filter.filter_area( scale_);

		for( int y = r.begin(), ye = r.end(); y < ye; ++y)
		{
			int src_y = Imath::clamp( y + dst_area_.min.y, src_area_.min.y, src_area_.max.y);
			image::const_image_view_t::x_iterator src_it( src_.row_begin( src_y - src_area_.min.y));
			image::image_view_t::x_iterator dst_it( dst_.row_begin( y));

			for (int x = 0, xe = dst_.width(); x < xe; ++x)
			{
				float old_x = resize_coord( x + dst_area_.min.x, xcenter_, scale_);
				int old_xi = Imath::Math<float>::floor( old_x);

				int min_x = std::max( src_area_.min.x, old_xi - filter_area / 2 + 1);
				int max_x = std::min( old_xi + filter_area / 2, src_area_.max.x);

				__m128 acc = _mm_setzero_ps();
				float total_weight = 0;

				// iterate over the filter box
				for( int dx = min_x; dx <= max_x; dx++)
				{
					float weight = filter( ( dx - old_x) / filter_width);
					total_weight += weight;

					__m128 p = _mm_load_ps( reinterpret_cast<const float*>( &(src_it[ dx - src_area_.min.x])));
					__m128 w = _mm_set_ps1( weight);
					p = _mm_mul_ps( p, w);
					acc = _mm_add_ps( acc, p);
				}

				if (total_weight > 0)
				{
					__m128 w = _mm_set_ps1( 1.0f / total_weight);
					acc = _mm_mul_ps( acc, w);
				}

				_mm_store_ps( reinterpret_cast<float*>( &( *dst_it)), acc);
				dst_it++;
			}
		}
	}

private:

	float scale_;
	float xcenter_;

	const image::const_image_view_t& src_;
	Imath::Box2i src_area_;

	const image::image_view_t& dst_;
	Imath::Box2i dst_area_;
};

template<class Filter>
struct resize_y_fun
{
	resize_y_fun( const image::const_image_view_t& src, int src_min, int src_max,
					const image::image_view_t& dst, int dst_min, int dst_max,
				  int ycenter, float scale) : src_( src), dst_( dst)
	{
		RAMEN_ASSERT( src.width() == dst.width());
		RAMEN_ASSERT( src.height() == src_max - src_min + 1);
		RAMEN_ASSERT( dst.height() == dst_max - dst_min + 1);

		scale_ = scale;
		src_min_ = src_min;
		src_max_ = src_max;
		dst_min_ = dst_min;
		dst_max_ = dst_max;
		ycenter_ = ycenter;
	}

	void operator()( const tbb::blocked_range<int>& r) const
	{
		using namespace boost::gil;

		Filter filter;

		float filter_width = std::max( 1.0f, scale_);
	    int filter_area = filter.filter_area( scale_);

		for (int x = r.begin(); x < r.end(); ++x)
		{
			image::const_image_view_t::y_iterator src_it( src_.col_begin( x));
			image::image_view_t::y_iterator dst_it( dst_.col_begin( x));

			for (int y = 0, ye = dst_.height(); y < ye; ++y)
			{
				float old_y = resize_coord( y + dst_min_, ycenter_, scale_);
				int old_yi = Imath::Math<float>::floor( old_y);

				int min_y = std::max( src_min_, old_yi - filter_area / 2 + 1);
				int max_y = std::min( old_yi + filter_area / 2, src_max_);

				__m128 acc = _mm_setzero_ps();
				float total_weight = 0;

				// iterate over the filter box
				for( int dy = min_y; dy <= max_y; ++dy)
				{
					float weight = filter( ( dy - old_y) / filter_width);
					total_weight += weight;

					__m128 p = _mm_load_ps( reinterpret_cast<const float*>( &(src_it[ dy - src_min_])));
					__m128 w = _mm_set_ps1( weight);
					p = _mm_mul_ps( p, w);
					acc = _mm_add_ps( acc, p);
				}

				if (total_weight > 0)
				{
					__m128 w = _mm_set_ps1( 1.0f / total_weight);
					acc = _mm_mul_ps( acc, w);
				}

				_mm_store_ps( reinterpret_cast<float*>( &( *dst_it)), acc);
				dst_it++;
			}
		}
	}

private:

	float scale_;
	int ycenter_;

	const image::const_image_view_t& src_;
	int src_min_, src_max_;

	const image::image_view_t& dst_;
	int dst_min_, dst_max_;
};

template<class Filter>
void resize( const image::const_image_view_t& src, const Imath::Box2i& src_defined, const Imath::Box2i& src_area,
			   const image::image_view_t& tmp, const Imath::Box2i& tmp_area,
			   const image::image_view_t& dst, const Imath::Box2i& dst_area,
			 const Imath::V2i& center, const Imath::V2f& scale)
{

	resize_x_fun<Filter> xf( src, src_defined, tmp, tmp_area, center.x, scale.x);
	resize_y_fun<Filter> yf( tmp, tmp_area.min.y, tmp_area.max.y, dst, dst_area.min.y, dst_area.max.y, center.y, scale.y);

	#ifndef NDEBUG
		xf( tbb::blocked_range<int>( 0, tmp.height()));
		yf( tbb::blocked_range<int>( 0, dst.width()));
	#else
		tbb::parallel_for( tbb::blocked_range<int>( 0, tmp.height()), xf);
		tbb::parallel_for( tbb::blocked_range<int>( 0, dst.width()), yf);
	#endif
}

/***********************************************************************/
// resize half

struct resize_half_fun
{
	resize_half_fun( const image::const_image_view_t& src, const Imath::Box2i& src_area,
						const image::image_view_t& dst, const Imath::Box2i& dst_area) : src_( src), dst_( dst)
	{
		src_area_ = src_area;
		dst_area_ = dst_area;

		float sum_weights = 0.0f;

		for( int j = 0; j < 3; ++j)
		{
			for( int i = 0; i < 3; ++i)
			{

				if( i == 1 && j == 1)
					weights_[j][i] = 2.0f;
				else
					weights_[j][i] = 1.0f;

				sum_weights += weights_[j][i];
			}
		}

		inv_total_weights_ = _mm_set_ps1( 1.0f / sum_weights);
	}

	void operator()( const tbb::blocked_range<int>& r) const
	{
		using namespace boost::gil;

		for( int y = r.begin(), ye = r.end(); y < ye; ++y)
		{
			image::image_view_t::x_iterator dst_it( dst_.row_begin( y));

			int src_y = 2 * ( y + dst_area_.min.y);

			for (int x = 0, xe = dst_.width(); x < xe; ++x)
			{
				int src_x = 2 * ( x + dst_area_.min.x);

				__m128 acc = _mm_setzero_ps();

				for( int j = -1; j <= 1; ++j)
				{
					int yy = Imath::clamp( src_y + j, src_area_.min.y, src_area_.max.y);
					image::const_image_view_t::x_iterator src_it( src_.row_begin( yy - src_area_.min.y));

					for( int i = -1; i <= 1; ++i)
					{
						int xx = Imath::clamp( src_x, src_area_.min.x, src_area_.max.x);
						float weight = weights_[j+1][i+1];

						__m128 p = _mm_load_ps( reinterpret_cast<const float*>( &(src_it[ xx - src_area_.min.x])));
						__m128 w = _mm_set_ps1( weight);

						p = _mm_mul_ps( p, w);
						acc = _mm_add_ps( acc, p);
					}
				}

				acc = _mm_mul_ps( acc, inv_total_weights_);
				_mm_store_ps( reinterpret_cast<float*>( &( *dst_it)), acc);
				dst_it++;
			}
		}
	}

private:

	const image::const_image_view_t& src_;
	Imath::Box2i src_area_;

	const image::image_view_t& dst_;
	Imath::Box2i dst_area_;

	float weights_[3][3];
	__m128 inv_total_weights_;
};

} // unnamed

void resize_bilinear( const const_image_view_t& src, const image_view_t& dst)
{
    generic::resize<sse2::bilinear_sampler_t>( src, dst);
}

void resize_lanczos3( const image::const_image_view_t& src, const Imath::Box2i& src_defined, const Imath::Box2i& src_area,
					   const image::image_view_t& tmp, const Imath::Box2i& tmp_area,
						const image::image_view_t& dst, const Imath::Box2i& dst_area,
					  const Imath::V2i& center, const Imath::V2f& scale)
{
	resize<lanczos3_filter_t>( src, src_defined, src_area, tmp, tmp_area, dst, dst_area, center, scale);
}

void resize_mitchell( const image::const_image_view_t& src, const Imath::Box2i& src_defined, const Imath::Box2i& src_area,
					   const image::image_view_t& tmp, const Imath::Box2i& tmp_area,
						const image::image_view_t& dst, const Imath::Box2i& dst_area,
					  const Imath::V2i& center, const Imath::V2f& scale)
{
	resize<mitchell_filter_t>( src, src_defined, src_area, tmp, tmp_area, dst, dst_area, center, scale);
}

void resize_catrom( const image::const_image_view_t& src, const Imath::Box2i& src_defined, const Imath::Box2i& src_area,
					   const image::image_view_t& tmp, const Imath::Box2i& tmp_area,
						const image::image_view_t& dst, const Imath::Box2i& dst_area,
					  const Imath::V2i& center, const Imath::V2f& scale)
{
	resize<catrom_filter_t>( src, src_defined, src_area, tmp, tmp_area, dst, dst_area, center, scale);
}

void resize_half( const image::const_image_view_t& src, const Imath::Box2i& src_area,
					const image::image_view_t& dst, const Imath::Box2i& dst_area)
{
	tbb::parallel_for( tbb::blocked_range<int>( 0, dst.height()), resize_half_fun( src, src_area, dst, dst_area));
}

} // namespace
} // namespace
} // namespace
