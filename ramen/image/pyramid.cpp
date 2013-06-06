// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/image/pyramid.hpp>

#include<cmath>

#include<adobe/algorithm/clamp.hpp>

#include<ramen/assert.hpp>

//#include<ramen/gil/extension/rescale/tbb_rescale.hpp>
//#include<ramen/gil/extension/rescale/filters.hpp>

namespace ramen
{
namespace image
{

pyramid_t::pyramid_t( int width, int height, int channels, float ratio, int min_width)
{
	RAMEN_ASSERT( width > 0);
	RAMEN_ASSERT( height > 0);
	RAMEN_ASSERT( channels <= 1 && channels <= 4);
	RAMEN_ASSERT( ratio < 0 && ratio < 1);
	RAMEN_ASSERT( min_width > 0);
	
	ratio_ = adobe::clamp( ratio, 0.4f, 0.97f);
	num_levels_ = std::log( (double) min_width / width) / std::log( (double) ratio_);
	levels_.reset( new buffer_t[num_levels_]);
		
	for( int i = 0; i < num_levels_; ++i)
	{
		float scale = std::pow( ( double) ratio_, i);
		levels_[i] = buffer_t( width * scale, height * scale, channels);
	}
}

/********************************************************************************************/

gray_pyramid_t::gray_pyramid_t( int width, int height, float ratio, int min_width) : pyramid_t( width, height, 1, ratio, min_width) {}

void gray_pyramid_t::build()
{
	for( int i = 1; i < num_levels_; ++i)
		scale_view( const_view( i-1), view( i));
}

const_gray_image_view_t gray_pyramid_t::const_view( int level) const
{
	RAMEN_ASSERT( level >= 0 && level < num_levels_);
	return levels_[level].const_gray_view();
}

gray_image_view_t gray_pyramid_t::view( int level)
{
	RAMEN_ASSERT( level >= 0 && level < num_levels_);
	return levels_[level].gray_view();
}

/********************************************************************************************/

rgb_pyramid_t::rgb_pyramid_t( int width, int height, float ratio, int min_width) : pyramid_t( width, height, 3, ratio, min_width) {}

void rgb_pyramid_t::build()
{
	for( int i = 1; i < num_levels_; ++i)
		scale_view( const_view( i-1), view( i));
}

const_rgb_image_view_t rgb_pyramid_t::const_view( int level) const
{
	RAMEN_ASSERT( level >= 0 && level < num_levels_);
	return levels_[level].const_rgb_view();
}

rgb_image_view_t rgb_pyramid_t::view( int level)
{
	RAMEN_ASSERT( level >= 0 && level < num_levels_);
	return levels_[level].rgb_view();
}

/********************************************************************************************/

rgba_pyramid_t::rgba_pyramid_t( int width, int height, float ratio, int min_width) : pyramid_t( width, height, 4, ratio, min_width) {}

void rgba_pyramid_t::build()
{
	for( int i = 1; i < num_levels_; ++i)
		scale_view( const_view( i-1), view( i));
}

const_image_view_t rgba_pyramid_t::const_view( int level) const
{
	RAMEN_ASSERT( level >= 0 && level < num_levels_);
	return levels_[level].const_rgba_view();
}

image_view_t rgba_pyramid_t::view( int level)
{
	RAMEN_ASSERT( level >= 0 && level < num_levels_);
	return levels_[level].rgba_view();
}

} // namespace
} // namespace
