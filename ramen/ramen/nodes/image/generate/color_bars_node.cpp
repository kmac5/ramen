// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/nodes/image/generate/color_bars_node.hpp>

#include<ramen/image/color_bars.hpp>

namespace ramen
{
namespace image
{

color_bars_node_t::color_bars_node_t() : generator_node_t() { set_name( "color bars");}

void color_bars_node_t::do_create_params()
{
    generator_node_t::do_create_params();
}

void color_bars_node_t::do_process( const render::context_t& context)
{
    image::make_color_bars( image_view(), format(), defined());
}

// factory
node_t *create_color_bars_node() { return new color_bars_node_t();}

const node_metaclass_t *color_bars_node_t::metaclass() const { return &color_bars_node_metaclass();}

const node_metaclass_t& color_bars_node_t::color_bars_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.color_bars";
        info.major_version = 1;
        info.minor_version = 0;
        info.menu = "Image";
        info.submenu = "Input";
        info.menu_item = "Color Bars";
        info.create = &create_color_bars_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( color_bars_node_t::color_bars_node_metaclass());

} // namespace
} // namespace
