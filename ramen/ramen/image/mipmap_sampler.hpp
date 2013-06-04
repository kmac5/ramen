// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_IMAGE_MIPMAP_HPP
#define RAMEN_IMAGE_MIPMAP_HPP

#include<vector>

#include<OpenEXR/ImathVec.h>
#include<OpenEXR/ImathMath.h>

#include<ramen/image/typedefs.hpp>

#include<ramen/assert.hpp>

#include<ramen/math/fast_float.hpp>

namespace ramen
{
namespace image
{

template<class Sampler, class TopLevelSampler = Sampler>
class mipmap_sampler_t
{
public:

	mipmap_sampler_t() : has_top_sampler_( false) {}

	explicit mipmap_sampler_t( const TopLevelSampler& sampler) { set_top_level( sampler);}

	int min_size() const { return 32;}

	int num_levels() const { return samplers_.size() + 1;}

	void set_top_level( const TopLevelSampler& sampler)
	{
		RAMEN_ASSERT( !has_top_sampler_);

		top_sampler_ = sampler;
		has_top_sampler_ = true;

		scales_.push_back( 1.0f);
		iscales_.push_back( 1.0f);
	}

	void add_level( const Sampler& sampler)
	{
		RAMEN_ASSERT( has_top_sampler_);

		samplers_.push_back( sampler);

		float scale = scales_.back() * 0.5f;
		scales_.push_back( scale);
		iscales_.push_back( 1.0f / scale);
	}

	const Imath::Box2i& src_area() const
	{
		RAMEN_ASSERT( has_top_sampler_);
		return top_sampler_.src_area();
	}

	const std::vector<Sampler>& samplers() const { return samplers_;}

    pixel_t operator()( const vector2_t& p) const
	{
		RAMEN_ASSERT( has_top_sampler_);
		return top_sampler_( p);
	}

    pixel_t operator()( const vector2_t& p, const vector2_t& du, const vector2_t& dv) const
	{
		RAMEN_ASSERT( has_top_sampler_);

		int y = math::fast_float_floor( p.y);

		if( y < src_area().min.y || y > src_area().max.y)
	        return pixel_t( 0, 0, 0, 0);

		int x = math::fast_float_floor( p.x);

		if( x < src_area().min.x || x > src_area().max.x)
	        return pixel_t( 0, 0, 0, 0);

		float max_width = std::max( Imath::Math<float>::fabs( du.x),  std::max( Imath::Math<float>::fabs( du.y),
							std::max( Imath::Math<float>::fabs( dv.x), Imath::Math<float>::fabs( dv.y))));

		float width = 2.0f * max_width;

		float fract;
		int level = find_level_and_fract( width, fract);
		image::pixel_t result = sample_level( level, p);

		if( fract != 0.0f)
		{
			RAMEN_ASSERT( level < num_levels() - 1);

			image::pixel_t tmp = sample_level( level + 1, p);
			result = lerp_pixels( tmp, result, fract);
		}

		return result;
	}

public:

	int find_level_and_fract( float width, float& fract) const
	{
		if( width <= 1.0f)
		{
			fract = 0;
			return 0;
		}

		float scale = 1.0f;

		for( int i = 0, ie = num_levels() - 1; i < ie; ++i)
		{
			float next_scale = iscales_[i+1];

			if( width >= scale && width < next_scale)
			{
				fract = ( width -  scale) / ( float) ( next_scale - scale);
				return i;
			}

			scale = next_scale;
		}

		fract = 0;
		return num_levels() - 1;
	}

	pixel_t sample_level( int level, const vector2_t& p) const
	{
		RAMEN_ASSERT( level >= 0 && level < num_levels());

		if( level == 0)
			return top_sampler_( p);

		float s = scales_[level];
		vector2_t q( p.x * s, p.y * s);
		return samplers_[level - 1]( q);
	}

	pixel_t lerp_pixels( const pixel_t& p, const pixel_t& q, float t) const
	{
		float it = 1.0f - t;
	    return pixel_t( boost::gil::get_color( p, boost::gil::red_t())   * t + boost::gil::get_color( q, boost::gil::red_t())   * it,
						boost::gil::get_color( p, boost::gil::green_t()) * t + boost::gil::get_color( q, boost::gil::green_t()) * it,
						boost::gil::get_color( p, boost::gil::blue_t())  * t + boost::gil::get_color( q, boost::gil::blue_t())  * it,
						boost::gil::get_color( p, boost::gil::alpha_t()) * t + boost::gil::get_color( q, boost::gil::alpha_t()) * it);
	}

	bool has_top_sampler_;
	TopLevelSampler top_sampler_;
	std::vector<Sampler> samplers_;
	std::vector<float> scales_;
	std::vector<float> iscales_;
};

} // namespace
} // namespace

#endif
