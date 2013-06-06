// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/image/resize.hpp>
#include<ramen/image/processing.hpp>

#include<ramen/image/buffer.hpp>

#include<ramen/image/generic/resize.hpp>

namespace ramen
{
namespace image
{
namespace
{

template<class Filter>
Imath::Box2i calc_tmp_area( const Imath::Box2i& src_area, const Imath::Box2i& dst_area, float scale)
{
	Filter filter;
	int filter_area = filter.filter_area( scale);

	Imath::Box2i tmp_area( dst_area);
	tmp_area.min.y = src_area.min.y - filter_area / 2 + 1;
	tmp_area.max.y = src_area.max.y + filter_area / 2;
	return tmp_area;
}

} // unnamed

void resize_lanczos3( const image::const_image_view_t& src, const Imath::Box2i& src_defined,
					   const image::image_view_t& dst, const Imath::Box2i& dst_area,
					  const Imath::V2i& center, const Imath::V2f& scale)
{
	Imath::Box2i src_area( resize_box( dst_area, center, scale.x, scale.y));
	Imath::Box2i tmp_area( calc_tmp_area<lanczos3_filter_t>( src_area, dst_area, scale.y));
	image::buffer_t tmp( tmp_area, 4);
	resize_lanczos3_( src, src_defined, src_area, tmp.rgba_view(), tmp_area, dst, dst_area, center, scale);
}

void resize_mitchell( const image::const_image_view_t& src, const Imath::Box2i& src_defined,
					   const image::image_view_t& dst, const Imath::Box2i& dst_area,
					  const Imath::V2i& center, const Imath::V2f& scale)
{
	Imath::Box2i src_area( resize_box( dst_area, center, scale.x, scale.y));
	Imath::Box2i tmp_area( calc_tmp_area<lanczos3_filter_t>( src_area, dst_area, scale.y));
	image::buffer_t tmp( tmp_area, 4);
	resize_mitchell_( src, src_defined, src_area, tmp.rgba_view(), tmp_area, dst, dst_area, center, scale);
}

void resize_catrom( const image::const_image_view_t& src, const Imath::Box2i& src_defined,
					   const image::image_view_t& dst, const Imath::Box2i& dst_area,
					  const Imath::V2i& center, const Imath::V2f& scale)
{
	Imath::Box2i src_area( resize_box( dst_area, center, scale.x, scale.y));
	Imath::Box2i tmp_area( calc_tmp_area<lanczos3_filter_t>( src_area, dst_area, scale.y));
	image::buffer_t tmp( tmp_area, 4);
	resize_catrom_( src, src_defined, src_area, tmp.rgba_view(), tmp_area, dst, dst_area, center, scale);
}

} // namespace
} // namespace
