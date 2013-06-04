// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_TRACKER_NCC_TRACKER_IMPL_HPP
#define RAMEN_TRACKER_NCC_TRACKER_IMPL_HPP

#include<tbb/blocked_range.h>
#include<tbb/parallel_reduce.h>
#include<tbb/parallel_for.h>

#include<OpenEXR/ImathBox.h>
#include<OpenEXR/ImathMath.h>

#include<ramen/assert.hpp>

#include<ramen/ImathExt/ImathBoxAlgo.h>

#include<ramen/params/param_set.hpp>

#include<ramen/image/buffer.hpp>
#include<ramen/image/color.hpp>

#include<ramen/image/cimg.hpp>
using namespace cimg_library;

#include<iostream>

namespace ramen
{
namespace track
{

struct ncc_tracker_t::impl
{
public:

	impl( float adapt_tolerance)
	{
		ref_valid_ = false;
		adapt_tolerance_ = adapt_tolerance;
		ncc_coeff_ = 0;
		confidence_ = 0;
	}

	void clear_images()
	{
		invalidate_reference();
		reference_.assign();
		search_.assign();
	}

	bool valid_reference() const { return ref_valid_;}
	
	void invalidate_reference()
	{
		ref_valid_ = false;
		ncc_coeff_ = 0;
	}
	
	void set_adapt_tolerance( float tol)
	{
		RAMEN_ASSERT( tol >= 0.0f && tol <= 1.0f);
		
		adapt_tolerance_ = tol;
		
		if( ref_valid_ && ncc_coeff_ < adapt_tolerance_)
			invalidate_reference();
	}
		
	void set_reference_pos( const Imath::V2f& p)
	{
		ref_pos_ = Imath::V2i( Imath::Math<float>::floor( p.x),
							   Imath::Math<float>::floor( p.y));
	}
	
	void update_reference( const Imath::Box2f& area, const image::buffer_t& pixels)
	{
		Imath::Box2i ref_area = Imath::Box2i( Imath::V2i( Imath::Math<float>::floor( area.min.x),
														  Imath::Math<float>::floor( area.min.y)),
												Imath::V2i( Imath::Math<float>::ceil( area.max.x),
														    Imath::Math<float>::ceil( area.max.y)));
		
		ref_area = Imath::intersect( ref_area, pixels.bounds());
	
		if( ref_area.isEmpty())
		{
			reference_.assign();
			return;
		}
		
		ref_pos_ = ref_area.min;
		ref_offset_ = Imath::V2i( 0, 0);

		// add an extra row and col for interpolation and gradients.
		ref_width_ = ref_area.size().x + 2;
		ref_height_ = ref_area.size().y + 2;

		if( reference_.width() != ref_width_ || reference_.height() != ref_height_ || reference_.spectrum() != 1)
			reference_ = CImg<>( ref_width_, ref_height_, 1, 1, 0);

		do_copy_area( pixels, ref_pos_.x, ref_pos_.y, ref_width_, ref_height_, reference_);
		--ref_width_;
		--ref_height_;
		ref_valid_ = true;
	}

	Imath::V2f transform_ncc_match( float x, float y) const
	{
		return Imath::V2f( search_pos_.x + x - ref_pos_.x + ref_offset_.x,
						   search_pos_.y + y - ref_pos_.y + ref_offset_.y);
	}
	
	void update_search( const Imath::Box2f& area, const image::buffer_t& pixels)
	{
		Imath::Box2i search_area = Imath::Box2i( Imath::V2i( Imath::Math<float>::floor( area.min.x),
															 Imath::Math<float>::floor( area.min.y)),
												Imath::V2i( Imath::Math<float>::ceil( area.max.x),
														    Imath::Math<float>::ceil( area.max.y)));
		
		search_area = Imath::intersect( search_area, pixels.bounds());
	
		if( search_area.isEmpty())
		{
			search_.assign();
			return;
		}
		
		search_pos_ = search_area.min;
		search_offset_ = Imath::V2i( 1, 1);
		
		RAMEN_ASSERT( ref_pos_.x <= search_area.max.x);
		RAMEN_ASSERT( ref_pos_.y <= search_area.max.y);

		if( ref_pos_.x < search_pos_.x)
		{
			ref_offset_.x = search_pos_.x - ref_pos_.x;
			ref_pos_.x = search_pos_.x;
		}
		
		if( ref_pos_.y < search_pos_.y)
		{
			ref_offset_.y = search_pos_.y - ref_pos_.y;
			ref_pos_.y = search_pos_.y;
		}
		
		ref_width_  -= ref_offset_.x;
		ref_height_ -= ref_offset_.y;
		
		ref_width_  = std::min( ref_width_ , search_area.size().x + 1);
		ref_height_ = std::min( ref_height_, search_area.size().y + 1);
		
		// add a one pixel margin for interpolation and gradients.
		search_width_ = search_area.size().x + 3;
		search_height_ = search_area.size().y + 3;
				
		if( search_.width() != search_width_ || search_.height() != search_height_ || search_.spectrum() != 2)
			search_ = CImg<>( search_width_, search_height_, 1, 2, 0);

		do_copy_area( pixels, search_pos_.x - 1, search_pos_.y - 1, search_width_, search_height_, search_);
		make_integral_image( search_);
		search_width_  -= 2;
		search_height_ -= 2;		
	}
	
	Imath::V2f track()
	{
		if( reference_.is_empty() || search_.is_empty())
			return Imath::V2f( 0, 0);

		ref_mean_ = calc_reference_mean();

		int search_w = search_width_  - ref_width_;
		int search_h = search_height_ - ref_height_;

		if( ncc_result_.width() != search_w || ncc_result_.height() != search_h || ncc_result_.spectrum() != 1)
			ncc_result_ = CImg<>( search_w, search_h);

		tbb::parallel_for( tbb::blocked_range<int>( 0, search_h), boost::bind( &ncc_tracker_t::impl::do_track, this, _1));
		Imath::V2i best_int_ncc = find_best_ncc_match();

		// subpixel search

		const int subpixel_res = 6;
		const float subpixel_inc = 1.0f / subpixel_res;
		ncc_coeff_ = -2;
		float best_dist = std::numeric_limits<float>::max();
		float max_ncc_x, max_ncc_y;

		for( float j = -1.0f + subpixel_inc; j < 1.0f; j += subpixel_inc)
		{
			for( float i = -1.0f + subpixel_inc; i < 1.0f; i += subpixel_inc)
			{
				float c = subpixel_ncc( best_int_ncc.x + i, best_int_ncc.y + j, ref_mean_, subpixel_res);

				if( c > ncc_coeff_)
				{
					ncc_coeff_ = c;
					max_ncc_x = best_int_ncc.x + i;
					max_ncc_y = best_int_ncc.y + j;
					best_dist = transform_ncc_match( max_ncc_x, max_ncc_y).length2();
				}
				else
				{
					if( c == ncc_coeff_)
					{
						float dist2 = transform_ncc_match( best_int_ncc.x + i, best_int_ncc.y + j).length2();

						if( dist2 < best_dist)
						{
							ncc_coeff_ = c;
							max_ncc_x = best_int_ncc.x + i;
							max_ncc_y = best_int_ncc.y + j;
							best_dist = dist2;
						}
					}
				}
			}
		}

		ncc_coeff_ = std::max( ncc_coeff_, 0.0f);
		confidence_ = ncc_coeff_;

		if( ncc_coeff_ < adapt_tolerance_)
			invalidate_reference();

		return transform_ncc_match( max_ncc_x, max_ncc_y);
	}

	float confidence() const { return confidence_;}
	
private:
	
	// copy
	void do_copy_area( const image::buffer_t& src, int xmin, int ymin, int w, int h, CImg<>& dst)
	{
		RAMEN_ASSERT( dst.width() == w);
		RAMEN_ASSERT( dst.height() == h);
		RAMEN_ASSERT( dst.spectrum() > 0);
		
		image::const_image_view_t view( src.const_rgba_view());
		Imath::Box2i bounds = src.bounds();
				
		for( int j = 0; j < h; ++j)
		{
			for( int i = 0; i < w; ++i)
			{
				int x = adobe::clamp( i + xmin - bounds.min.x, 0, (int) view.width()  - 1);
				int y = adobe::clamp( j + ymin - bounds.min.y, 0, (int) view.height() - 1);
				dst( i, j) = image::luminance( view( x, y));
			}
		}
	}
	
	// stats
	float calc_reference_mean() const
	{
		RAMEN_ASSERT( reference_.width() != 0 && reference_.height() != 0);
		
		float mean = 0;
		int num_pixels = 0;
		
		for( int j = ref_offset_.y, je = ref_offset_.y + ref_height_ - 1; j < je; ++j)
		{
			for( int i = ref_offset_.x, ie = ref_offset_.x + ref_width_ - 1; i < ie; ++i)
			{
				mean += reference_( i, j);
				++num_pixels;
			}
		}
		
		return mean / num_pixels;
	}	
	
	// integral images
	void make_integral_image( CImg<>& I)
	{
		RAMEN_ASSERT( I.width() != 0 && I.height() != 0);
		RAMEN_ASSERT( I.spectrum() == 2);
		
		int w = I.width();
		int h = I.height();
	
		// first column & row
		I( 0, 0, 0, 1) = I( 0, 0, 0, 0);
		
		for( int j = 1; j < h; ++j)
			I( 0, j, 0, 1) = I( 0, j - 1, 0, 1) + I( 0, j, 0, 0);
		
		for( int i = 1; i < w; ++i)
			I( i, 0, 0, 1) = I( i - 1, 0, 0, 1) + I( i, 0, 0, 0);
	
		// rest of the table
		for( int j = 1; j < h; ++j)
			for( int i = 1; i < w; ++i)
				I( i, j, 0, 1) = I( i, j, 0, 0) + I( i - 1, j, 0, 1) + I( i, j - 1, 0, 1) - I( i - 1, j - 1, 0, 1);
	}
	
	float calc_mean( const CImg<>& table, int xmin, int ymin, int xmax, int ymax) const	
	{
		RAMEN_ASSERT( table.width() != 0 && table.height() != 0);
		RAMEN_ASSERT( table.spectrum() == 2);
		RAMEN_ASSERT( xmin >= 0 && xmin < table.width() && ymin >= 0 && ymin < table.height());
		RAMEN_ASSERT( xmax >= 0 && xmax < table.width() && ymax >= 0 && ymax < table.height());
		RAMEN_ASSERT( xmax > xmin && ymax > ymin);
		
		int area = ( xmax - xmin) * ( ymax - ymin);
		float a = table( xmin, ymin, 0, 1);
		float b = table( xmax, ymax, 0, 1);
		float c = table( xmax, ymin, 0, 1);
		float d = table( xmin, ymax, 0, 1);
		return ( a + b - c - d) / area;
	}

	float calc_subpixel_mean( const CImg<>& table, float xmin, float ymin, float xmax, float ymax) const
	{
		RAMEN_ASSERT( table.width() != 0 && table.height() != 0);
		RAMEN_ASSERT( table.spectrum() == 2);
		RAMEN_ASSERT( xmin >= 0 && xmin < table.width() && ymin >= 0 && ymin < table.height());
		RAMEN_ASSERT( xmax >= 0 && xmax < table.width() && ymax >= 0 && ymax < table.height());
		RAMEN_ASSERT( xmax > xmin && ymax > ymin);
		
		float area = ( xmax - xmin) * ( ymax - ymin);
		float a = table.linear_atXY( xmin, ymin, 0, 1);
		float b = table.linear_atXY( xmax, ymax, 0, 1);
		float c = table.linear_atXY( xmax, ymin, 0, 1);
		float d = table.linear_atXY( xmin, ymax, 0, 1);
		return ( a + b - c - d) / area;
	}
	
	// util
	inline float square( float x) const { return x*x;}

	// ncc

	float ncc( int search_x, int search_y, float ref_mean) const
	{	
		float search_mean = calc_mean( search_, search_x + search_offset_.x, 
									   search_y + search_offset_.y, 
									   search_x + search_offset_.x + ref_width_, 
									   search_y + search_offset_.y + ref_height_);
	
		float num = 0, ref_denom = 0, search_denom = 0, denom = 0;
	
		for( int j = 0; j < ref_height_; ++j)
		{
			for( int i = 0; i < ref_width_; ++i)
			{
				float ref_diff = reference_( ref_offset_.x + i, ref_offset_.y + j) - ref_mean;
				float s = search_( search_x + i + search_offset_.x,  
								   search_y + j + search_offset_.y);

				float search_diff = s - search_mean;
				num += ( ref_diff * search_diff);
				ref_denom += square( ref_diff);
				search_denom += square( search_diff);
			}
		}

		denom = std::sqrt( ref_denom) * std::sqrt( search_denom);
	
		if( denom)
			return num / denom;
		
		return -1.0f;
	}	
	
	float subpixel_ncc( float search_x, float search_y, float ref_mean, int subpixel_res) const
	{
		float inc = 1.0f / subpixel_res;
		float num = 0, ref_denom = 0, search_denom = 0, denom = 0;
		
		float search_mean = calc_subpixel_mean( search_, search_x + search_offset_.x, 
											   search_y + search_offset_.y, 
											   search_x + search_offset_.x + ref_width_, 
											   search_y + search_offset_.y + ref_height_);
		
		for( float j = 0; j < ref_height_; j += inc)
		{
			for( float i = 0; i < ref_width_; i += inc)
			{
				float ref_diff = reference_.linear_atXY( ref_offset_.x + i, ref_offset_.y + j) - ref_mean;
				float s = search_.linear_atXY( search_x + i + search_offset_.x,  
												search_y + j + search_offset_.y);

				float search_diff = s - search_mean;
				num += ( ref_diff * search_diff);
				ref_denom += square( ref_diff);
				search_denom += square( search_diff);
			}
		}
	
		denom = std::sqrt( ref_denom) * std::sqrt( search_denom);
	
		if( denom)
			return num / denom;
		
		return -1.0f;
	}	
	
	// parallel tracking

	void do_track( const tbb::blocked_range<int>& range)
	{
		for( int j = range.begin(); j < range.end(); ++j)
			for( int i = 0, ie = search_width_  - ref_width_; i < ie; ++i)
				ncc_result_( i, j) = ncc( i, j, ref_mean_);
	}

	Imath::V2f do_subixel_track( const tbb::blocked_range<int>& range, const Imath::V2i& best_int_ncc,
								 int subpixel_res, float& ncc_result, float& track_dist)
	{
		const float subpixel_inc = 1.0f / subpixel_res;

		Imath::V2f max_ncc( -1, -1);
		float ncc_coeff = -2.0f;

		float best_dist = std::numeric_limits<float>::max();

		for( int y = range.begin(); y < range.end(); ++y)
		{
			float j = -1.0f + ( subpixel_inc * y);

			for( int x = 0; x < subpixel_res; ++x)
			{
				float i = -1.0f + ( subpixel_inc * x);
				float c = subpixel_ncc( best_int_ncc.x + i, best_int_ncc.y + j, ref_mean_, subpixel_res);

				if( c > ncc_coeff)
				{
					ncc_coeff = c;
					max_ncc.x = best_int_ncc.x + i;
					max_ncc.y = best_int_ncc.y + j;
					best_dist = transform_ncc_match( max_ncc.x, max_ncc.y).length2();
				}
				else
				{
					if( c == ncc_coeff)
					{
						float dist2 = transform_ncc_match( best_int_ncc.x + i, best_int_ncc.y + j).length2();

						if( dist2 < best_dist)
						{
							ncc_coeff = c;
							max_ncc.x = best_int_ncc.x + i;
							max_ncc.y = best_int_ncc.y + j;
							best_dist = dist2;
						}
					}
				}
			}
		}

		ncc_result = ncc_coeff;
		track_dist = best_dist;
		return max_ncc;
	}

	struct subpixel_track_reduction_fun
	{
		subpixel_track_reduction_fun( ncc_tracker_t::impl *t, const Imath::V2i& best, int res)
		{
			pimpl_ = t;
			ncc_coeff = -2;
			best_distance_ = std::numeric_limits<float>::max();
			subpixel_res = res;
			best_int_pos = best;
		}

		subpixel_track_reduction_fun( subpixel_track_reduction_fun& other, tbb::split s)
		{
			pimpl_ = other.pimpl_;
			ncc_coeff = -2;
			best_distance_ = std::numeric_limits<float>::max();
			subpixel_res = other.subpixel_res;
			best_int_pos = other.best_int_pos;
		}

		void operator()( const tbb::blocked_range<int>& range)
		{
			pos = pimpl_->do_subixel_track( range, best_int_pos, subpixel_res, ncc_coeff, best_distance_);
		}

		void join( subpixel_track_reduction_fun& other)
		{
			if( ncc_coeff < other.ncc_coeff)
			{
				ncc_coeff = other.ncc_coeff;
				best_distance_ = other.best_distance_;
			}
			else
			{
				if( ncc_coeff == other.ncc_coeff)
				{
					if( other.best_distance_ < best_distance_)
					{
						ncc_coeff = other.ncc_coeff;
						best_distance_ = other.best_distance_;

					}
				}
			}
		}

		Imath::V2f pos;
		float ncc_coeff;

	private:

		int subpixel_res;
		Imath::V2i best_int_pos;
		ncc_tracker_t::impl *pimpl_;
		float best_distance_;
	};

	Imath::V2i find_best_ncc_match()
	{
		// preprocess ncc_result here.
		//ncc_result_.blur_median( 3);

		Imath::V2i best_match( -1, -1);
		int max_ncc_coeff = -2.0f;
		float best_dist = std::numeric_limits<float>::max();

		for( int j = 0, je = ncc_result_.height(); j < je; ++j)
		{
			for( int i = 0, ie = ncc_result_.width(); i < ie; ++i)
			{
				float c = ncc_result_( i, j);

				if( c > max_ncc_coeff)
				{
					max_ncc_coeff = c;
					best_match.x = i;
					best_match.y = j;
					best_dist = transform_ncc_match( i, j).length2();
				}
				else
				{
					if( c == max_ncc_coeff)
					{
						float dist2 = transform_ncc_match( i, j).length2();

						if( dist2 < best_dist)
						{
							max_ncc_coeff = c;
							best_match.x = i;
							best_match.y = j;
							best_dist = dist2;
						}
					}
				}
			}
		}

		return best_match;
	}

	// data
	float adapt_tolerance_;
	
	bool ref_valid_;
	Imath::V2i ref_pos_;
	Imath::V2i ref_offset_;
	int ref_width_, ref_height_;
	CImg<> reference_;
	float ref_mean_;

	Imath::V2f search_pos_;
	Imath::V2i search_offset_;
	int search_width_, search_height_;	
	CImg<> search_;
	
	CImg<> ncc_result_;
	float ncc_coeff_;
	float confidence_;
};

} // tracker
} // ramen

#endif
