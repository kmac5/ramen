// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/nodes/image/base_blur_node.hpp>

#include<OpenEXR/ImathMath.h>

#include<ramen/gil/extension/border_algorithm.hpp>

#include<ramen/params/float_param.hpp>
#include<ramen/params/float2_param.hpp>
#include<ramen/params/popup_param.hpp>

#include<ramen/image/box_blur.hpp>

namespace ramen
{
namespace image
{

base_blur_node_t::base_blur_node_t() : areaop_node_t() {}

bool base_blur_node_t::expand_defined() const { return true;}

Imath::V2f base_blur_node_t::adjust_blur_size( const Imath::V2f& size, int subsample) const
{
	Imath::V2f result;
    result.x = size.x / aspect_ratio() / subsample * proxy_scale().x;
    result.y = size.y / subsample * proxy_scale().y;
	return result;
}

Imath::V2i base_blur_node_t::round_blur_size( const Imath::V2f& size) const
{
	return Imath::V2i( Imath::Math<float>::ceil( size.x),
					   Imath::Math<float>::ceil( size.y));
}

void base_blur_node_t::copy_src_image( int in, const Imath::Box2i& area, blur_border_mode bmode)
{
    boost::gil::copy_pixels( input_as<image_node_t>( in)->const_subimage_view( area), subimage_view( area));

    if( bmode != border_black)
    {
        int border_x0 = area.min.x - defined().min.x;
        int border_y0 = area.min.y - defined().min.y;

        int border_x1 = defined().max.x - area.max.x;
        int border_y1 = defined().max.y - area.max.y;

        if( bmode == border_repeat)
            boost::gil::repeat_border_pixels( image_view(), border_x0, border_y0, border_x1, border_y1);
        else
            boost::gil::reflect_border_pixels( image_view(), border_x0, border_y0, border_x1, border_y1);
    }	
}

} // namespace
} // namespace
