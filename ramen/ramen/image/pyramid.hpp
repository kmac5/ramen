// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_IMAGE_PYRAMID_HPP
#define RAMEN_IMAGE_PYRAMID_HPP

#include<boost/noncopyable.hpp>
#include<boost/scoped_array.hpp>

#include<ramen/image/buffer.hpp>

//#include<ramen/gil/extension/rescale/tbb_rescale.hpp>
//#include<ramen/gil/extension/rescale/filters.hpp>

namespace ramen
{
namespace image
{

class pyramid_t : boost::noncopyable
{
protected:

	pyramid_t( int width, int height, int channels, float ratio = 0.5f, int min_width = 50);
	
	template<class SrcView, class DstView>
	void scale_view( const SrcView& src, const DstView& dst) const
	{
		//boost::gil::tbb_rescale( src, dst, boost::gil::bicubic_filter());
	}
	
	float ratio_;
	int num_levels_;

	boost::scoped_array<buffer_t> levels_;
};

class gray_pyramid_t : public pyramid_t
{
public:

	gray_pyramid_t( int width, int height, float ratio = 0.5f, int min_width = 50);

	void build();
	
	const_gray_image_view_t const_view( int level = 0) const;
	gray_image_view_t view( int level = 0);
};

class rgb_pyramid_t : public pyramid_t
{
public:

	rgb_pyramid_t( int width, int height, float ratio = 0.5f, int min_width = 50);

	void build();
	
	const_rgb_image_view_t const_view( int level = 0) const;
	rgb_image_view_t view( int level = 0);
};

class rgba_pyramid_t : public pyramid_t
{
public:

	rgba_pyramid_t( int width, int height, float ratio = 0.5f, int min_width = 50);

	void build();
	
	const_image_view_t const_view( int level = 0) const;
	image_view_t view( int level = 0);
};

} // namespace
} // namespace

#endif
