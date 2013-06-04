// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/nodes/image/generate/color_node.hpp>

#include<ramen/params/color_param.hpp>

namespace ramen
{
namespace image
{

color_node_t::color_node_t() : generator_node_t() { set_name( "color");}

void color_node_t::do_create_params()
{
    generator_node_t::do_create_params();

    std::auto_ptr<color_param_t> c( new color_param_t( "Color"));
    c->set_id( "color");
    c->set_default_value( Imath::Color4f( 0, 0, 0, 0));
    add_param( c);
}

void color_node_t::do_process( const render::context_t& context)
{
    Imath::Color4f c( get_value<Imath::Color4f>( param( "color")));

    image::pixel_t p;
    boost::gil::get_color( p, boost::gil::red_t())   = c.r;
    boost::gil::get_color( p, boost::gil::green_t()) = c.g;
    boost::gil::get_color( p, boost::gil::blue_t())  = c.b;
    boost::gil::get_color( p, boost::gil::alpha_t()) = c.a;
    boost::gil::fill_pixels( image_view(), p);
}

// factory
node_t *create_color_node() { return new color_node_t();}

const node_metaclass_t *color_node_t::metaclass() const { return &color_node_metaclass();}

const node_metaclass_t& color_node_t::color_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.color";
        info.major_version = 1;
        info.minor_version = 0;
        info.menu = "Image";
        info.submenu = "Input";
        info.menu_item = "Color";
        info.create = &create_color_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( color_node_t::color_node_metaclass());

} // namespace
} // namespace
