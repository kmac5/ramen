// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/nodes/image/color/colorspace_node.hpp>

#include<ramen/params/popup_param.hpp>

#include<ramen/image/processing.hpp>

namespace ramen
{
namespace image
{
namespace
{

enum
{
    rgb2yuv = 0,
    rgb2hsv,
    rgb2lab,
    yuv2rgb,
    hsv2rgb,
    lab2rgb
};
} // unnamed

colorspace_node_t::colorspace_node_t() : pointop_node_t() { set_name("cspace");}

void colorspace_node_t::do_create_params()
{
    std::auto_ptr<popup_param_t> p( new popup_param_t( "Convert"));
    p->set_id( "convert");
    p->menu_items() = boost::assign::list_of( "RGB to YUV")( "RGB to HSV")( "RGB to Lab")
											( "YUV to RGB")( "HSV to RGB")( "Lab to RGB");
    add_param( p);
}

void colorspace_node_t::do_process( const image::const_image_view_t& src, const image::image_view_t& dst, const render::context_t& context)
{
    switch( get_value<int>( param( "convert")))
    {
    case rgb2yuv:
		image::convert_rgb_to_yuv( src, dst);
    break;

    case rgb2hsv:
		image::convert_rgb_to_hsv( src, dst);
    break;

    case rgb2lab:
		image::convert_rgb_to_lab( src, dst);
    break;

    case yuv2rgb:
		image::convert_yuv_to_rgb( src, dst);
    break;

    case hsv2rgb:
		image::convert_hsv_to_rgb( src, dst);
    break;

    case lab2rgb:
		image::convert_lab_to_rgb( src, dst);
    break;
    }
}

// factory
node_t *create_colorspace_node() { return new colorspace_node_t();}

const node_metaclass_t *colorspace_node_t::metaclass() const { return &colorspace_node_metaclass();}

const node_metaclass_t& colorspace_node_t::colorspace_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.colorspace";
        info.major_version = 1;
        info.minor_version = 0;
		info.menu = "Image";
        info.submenu = "Color";
        info.menu_item = "Colorspace";
        info.help = "Converts between RGB, HSV, Lab and other color spaces.";
        info.create = &create_colorspace_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( colorspace_node_t::colorspace_node_metaclass());

} // namespace
} // namespace
