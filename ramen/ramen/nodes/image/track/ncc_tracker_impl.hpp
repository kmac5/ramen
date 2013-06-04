// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_TRACKER_NCC_TRACKER_IMPL_HPP
#define RAMEN_TRACKER_NCC_TRACKER_IMPL_HPP

#include<tbb/blocked_range.h>
#include<tbb/parallel_for.h>

#include<OpenEXR/ImathBox.h>
#include<OpenEXR/ImathMath.h>

#include<ramen/assert.hpp>

#include<ramen/ImathExt/ImathBoxAlgo.h>

#include<ramen/params/param_set.hpp>

#include<ramen/image/buffer.hpp>
#include<ramen/image/color.hpp>
#include<ramen/image/sse2/samplers.hpp>

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
		confidence_ = 0;

		int size = 2 * ( subpixel_res() + 1) + 1;
		subpixel_ncc_result_ = CImg<>( size, size);
	}

	void clear_images()
	{
		invalidate_reference();
		reference_.assign();
		search_.assign();
	}

	bool valid_reference() const { return ref_valid_;}
	
	void invalidate_reference() { ref_valid_ = false;}
	
	void set_adapt_tolerance( float tol)
	{
		RAMEN_ASSERT( tol >= 0.0f && tol <= 1.0f);
		
		adapt_tolerance_ = tol;
		
		if( ref_valid_ && confidence_ <= adapt_tolerance_)
			invalidate_reference();
	}
		
	void set_reference_pos( const Imath::V2f& p)
	{
		Imath::V2i  size( ref_area_.size());
		ref_area_.min = p;
		ref_area_.max = p + size;
	}
	
	void update_reference( const Imath::Box2f& area, const image::buffer_t& pixels)
	{
		float subx = area.min.x - Imath::Math<float>::floor( area.min.x);
		float suby = area.min.y - Imath::Math<float>::floor( area.min.y);

		Imath::Box2i iref_area = Imath::Box2i( Imath::V2i( Imath::Math<float>::floor( area.min.x),
														  Imath::Math<float>::floor( area.min.y)),
												Imath::V2i( Imath::Math<float>::floor( area.max.x),
														    Imath::Math<float>::floor( area.max.y)));

		iref_area = Imath::intersect( iref_area, pixels.bounds());
	
		if( iref_area.isEmpty())
		{
			reference_.assign();
			return;
		}

		ref_area_.min = Imath::V2f( iref_area.min.x + subx, iref_area.min.y + suby);
		ref_area_.max = ref_area_.min + iref_area.size();

		// add an extra row and col for interpolation and gradients.
		int ref_width = iref_area.size().x + 2;
		int ref_height = iref_area.size().y + 2;

		if( reference_.width() != ref_width || reference_.height() != ref_height || reference_.spectrum() != 1)
			reference_ = CImg<>( ref_width, ref_height, 1, 1, 0);

		do_copy_area( pixels, ref_area_.min.x, ref_area_.min.y, ref_width, ref_height, reference_);
		ref_valid_ = true;
	}

	Imath::V2f transform_ncc_match( float x, float y, const Imath::V2f& ref_pos, const Imath::V2i& ref_offset) const
	{
		return Imath::V2f( search_pos_.x + x - ref_pos.x + ref_offset.x,
						   search_pos_.y + y - ref_pos.y + ref_offset.y);
	}
	
	void update_search( const Imath::Box2f& area, const image::buffer_t& pixels)
	{
		float subx = ref_area_.min.x - Imath::Math<float>::floor( ref_area_.min.x);
		float suby = ref_area_.min.y - Imath::Math<float>::floor( ref_area_.min.y);

		Imath::Box2i search_area = Imath::Box2i( Imath::V2i( Imath::Math<float>::floor( area.min.x),
															 Imath::Math<float>::floor( area.min.y)),
												Imath::V2i( Imath::Math<float>::floor( area.max.x),
														    Imath::Math<float>::floor( area.max.y)));
		
		search_area = Imath::intersect( search_area, pixels.bounds());
	
		if( search_area.isEmpty())
		{
			search_.assign();
			return;
		}
		
		search_pos_ = Imath::V2f( search_area.min.x + subx, search_area.min.y + suby);
		
		RAMEN_ASSERT( ref_area_.min.x <= search_area.max.x);
		RAMEN_ASSERT( ref_area_.min.y <= search_area.max.y);
		
		// add a one pixel margin for interpolation and gradients.
		search_width_ = search_area.size().x + 2;
		search_height_ = search_area.size().y + 2;
				
		if( search_.width() != search_width_ || search_.height() != search_height_ || search_.spectrum() != 2)
			search_ = CImg<>( search_width_, search_height_, 1, 2, 0);

		do_copy_area( pixels, search_pos_.x, search_pos_.y, search_width_, search_height_, search_);
		make_integral_image( search_);
		--search_width_;
		--search_height_;
	}
	
	int subpixel_res() const { return 8;}

	Imath::V2f track()
	{
		if( reference_.is_empty() || search_.is_empty())
			return Imath::V2f( 0, 0);

		Imath::V2f ref_pos = ref_area_.min;
		Imath::V2i ref_offset = Imath::V2i( 0, 0);
		int ref_width = ref_area_.size().x + 1;
		int ref_height = ref_area_.size().y + 1;

		if( ref_pos.x < search_pos_.x)
		{
			ref_offset.x = search_pos_.x - ref_pos.x;
			ref_pos.x += ref_offset.x;
		}

		if( ref_pos.y < search_pos_.y)
		{
			ref_offset.y = search_pos_.y - ref_pos.y;
			ref_pos.y += ref_offset.y;
		}

		ref_width  = std::min( ref_width - ref_offset.x, search_width_);
		ref_height = std::min( ref_height - ref_offset.y, search_height_);
		ref_mean_ = calc_reference_mean( ref_offset, ref_width, ref_height);

		int search_w = search_width_  - ref_width;
		int search_h = search_height_ - ref_height;

		if( ncc_result_.width() != search_w || ncc_result_.height() != search_h || ncc_result_.spectrum() != 1)
			ncc_result_ = CImg<>( search_w, search_h);

		// integer pixel search
		tbb::parallel_for( tbb::blocked_range<int>( 0, search_h), boost::bind( &ncc_tracker_t::impl::do_track, this, _1,
																			   ref_width, ref_height, ref_offset));
		Imath::V2i best_int_ncc = find_best_ncc_match( ncc_result_, confidence_);
		Imath::V2f max_ncc = best_int_ncc;

		if( confidence_ < 0.99f)
		{
			// subpixel search
			tbb::parallel_for( tbb::blocked_range<int>( 0, 2 * ( subpixel_res() + 1) + 1),
							   boost::bind( &ncc_tracker_t::impl::do_subpixel_track, this, _1, best_int_ncc, subpixel_res(),
											ref_width, ref_height, ref_offset));

			float subp_confidence;
			Imath::V2f best_subp_pos = subpixel_transform( find_best_ncc_match( subpixel_ncc_result_, subp_confidence)) + best_int_ncc;

			if( subp_confidence > confidence_)
			//if( 0)
			{
				max_ncc = best_subp_pos;
				confidence_ = subp_confidence;
			}
		}

		confidence_ = adobe::clamp( confidence_, 0.0f, 1.0f);

		if( confidence_ <= adapt_tolerance_)
			invalidate_reference();
		
		return transform_ncc_match( max_ncc.x, max_ncc.y, ref_pos, ref_offset);
	}

	float confidence() const { return confidence_;}
	
private:
	
	// copy
	void do_copy_area( const image::buffer_t& src, float xmin, float ymin, int w, int h, CImg<>& dst)
	{
		RAMEN_ASSERT( dst.width() == w);
		RAMEN_ASSERT( dst.height() == h);
		RAMEN_ASSERT( dst.spectrum() > 0);

		image::const_image_view_t view( src.const_rgba_view());
		Imath::Box2i bounds = src.bounds();

		image::sse2::bilinear_sampler_t sampler_( bounds, view);
		image::generic::repeat_sampler_t<image::sse2::bilinear_sampler_t> sampler( sampler_);

		for( int j = 0; j < h; ++j)
			for( int i = 0; i < w; ++i)
				dst( i, j) = image::luminance( sampler( image::vector2_t( i + xmin, j + ymin)));
	}

	// stats
	float calc_reference_mean( const Imath::V2i& ref_offset, int ref_width, int ref_height) const
	{
		RAMEN_ASSERT( reference_.width() != 0 && reference_.height() != 0);
		
		float mean = 0;
		int num_pixels = 0;
		
		for( int j = ref_offset.y, je = ref_offset.y + ref_height - 1; j < je; ++j)
		{
			for( int i = ref_offset.x, ie = ref_offset.x + ref_width - 1; i < ie; ++i)
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

	// tracking

	float ncc( int search_x, int search_y, float ref_mean, int ref_width, int ref_height, const Imath::V2i& ref_offset) const
	{	
		float search_mean = calc_mean( search_, search_x, search_y,
									   search_x + ref_width,
									   search_y + ref_height);
	
		float num = 0, ref_denom = 0, search_denom = 0, denom = 0;
	
		for( int j = 0; j < ref_height; ++j)
		{
			for( int i = 0; i < ref_width; ++i)
			{
				float ref_diff = reference_( ref_offset.x + i, ref_offset.y + j) - ref_mean;
				float s = search_( search_x + i, search_y + j);
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
	
	float subpixel_ncc( float search_x, float search_y, float ref_mean, float subpixel_inc, int ref_width, int ref_height, const Imath::V2i& ref_offset) const
	{
		if( search_x < 0 || search_y < 0 || search_x + ref_width >= search_.width() || search_y + ref_height >= search_.height())
			return 0;

		float num = 0, ref_denom = 0, search_denom = 0, denom = 0;

		float search_mean = calc_subpixel_mean( search_, search_x, search_y, search_x + ref_width, search_y + ref_height);
		
		for( float j = 0; j < ref_height; j += subpixel_inc)
		{
			for( float i = 0; i < ref_width; i += subpixel_inc)
			{
				float ref_diff = reference_.linear_atXY( ref_offset.x + i, ref_offset.y + j) - ref_mean;
				float s = search_.linear_atXY( search_x + i, search_y + j);

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

	Imath::V2f do_track( const tbb::blocked_range<int>& range, int ref_width, int ref_height, const Imath::V2i ref_offset)
	{
		for( int j = range.begin(); j < range.end(); ++j)
			for( int i = 0, ie = search_width_ - ref_width; i < ie; ++i)
				ncc_result_( i, j) = ncc( i, j, ref_mean_, ref_width, ref_height, ref_offset);
	}

	void do_subpixel_track( const tbb::blocked_range<int>& range, const Imath::V2i& best_int_ncc, int subpixel_res,
							int ref_width, int ref_height, const Imath::V2i ref_offset)
	{
		const float subpixel_inc = 1.0f / ( subpixel_res + 1);

		Imath::V2f max_ncc( -1, -1);
		float ncc_coeff = -2.0f;

		for( int y = range.begin(); y < range.end(); ++y)
		{
			float j = -1.0f + ( subpixel_inc * y);

			for( int x = 0, xe = 2 * ( subpixel_res + 1); x <= xe; ++x)
			{
				float i = -1.0f + ( subpixel_inc * x);
				subpixel_ncc_result_( x, y) = subpixel_ncc( best_int_ncc.x + i, best_int_ncc.y + j, ref_mean_,
															subpixel_inc, ref_width, ref_height, ref_offset);
			}
		}
	}

	Imath::V2i find_best_ncc_match( CImg<>& coeff_image, float& max_ncc)
	{
		//coeff_image.blur_median( 0.33);

		Imath::V2i match_pos( -1, -1);
		float max_ncc_coeff = -2.0f;

		for( int j = 0, je = coeff_image.height(); j < je; ++j)
		{
			for( int i = 0, ie = coeff_image.width(); i < ie; ++i)
			{
				float c = coeff_image( i, j);

				if( c > max_ncc_coeff)
				{
					max_ncc_coeff = c;
					match_pos.x = i;
					match_pos.y = j;
				}
			}
		}

		max_ncc = max_ncc_coeff;
		return match_pos;
	}

	Imath::V2f subpixel_transform( const Imath::V2i& p) const
	{
		float size = 2 * ( subpixel_res() + 1);
		return Imath::V2f( (( p.x / size) - 0.5f ) * 2.0f,
						   (( p.y / size) - 0.5f ) * 2.0f);
	}

	// data
	float adapt_tolerance_;
	
	bool ref_valid_;
	CImg<> reference_;
	float ref_mean_;
	Imath::Box2f ref_area_;

	Imath::V2f search_pos_;
	int search_width_, search_height_;	
	CImg<> search_;
	
	CImg<> ncc_result_;
	CImg<> subpixel_ncc_result_;
	float confidence_;
};

} // tracker
} // ramen

#endif
