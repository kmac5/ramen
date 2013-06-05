// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/nodes/image/color/adjust_hsv_node.hpp>

#include<ramen/ImathExt/ImathMatrixColorAlgo.h>

#include<ramen/image/processing.hpp>

#include<ramen/params/float_param.hpp>

namespace ramen
{
namespace image
{

adjust_hsv_node_t::adjust_hsv_node_t() : pointop_node_t() { set_name("adjust_hsv");}

void adjust_hsv_node_t::do_create_params()
{
    std::auto_ptr<float_param_t> p( new float_param_t( "Hue rot"));
    p->set_id( "hue_rot");
    p->set_default_value( 0);
    p->set_step( 0.5f);
    add_param( p);

    p.reset( new float_param_t( "Saturation"));
    p->set_id( "sat");
    p->set_default_value( 1);
    p->set_step( 0.05f);
    add_param( p);

    p.reset( new float_param_t( "Value"));
    p->set_id( "value");
    p->set_min( 0);
    p->set_default_value( 1);
    p->set_step( 0.05f);
    add_param( p);
}

void adjust_hsv_node_t::do_process( const image::const_image_view_t& src, const image::image_view_t& dst, const render::context_t& context)
{
    Imath::M44f m = ImathExt::hueRotationMatrix( get_value<float>( param( "hue_rot"))) *
                    ImathExt::saturationMatrix( get_value<float>( param( "sat"))) *
                    ImathExt::gainMatrix( get_value<float>( param( "value")));

    image::apply_color_matrix( src, dst, m);
}

// factory
node_t *create_adjust_hsv_node() { return new adjust_hsv_node_t();}

const node_metaclass_t *adjust_hsv_node_t::metaclass() const { return &adjust_hsv_node_metaclass();}

const node_metaclass_t& adjust_hsv_node_t::adjust_hsv_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t m;

    if( !inited)
    {
        m.id = "image.builtin.adjust_hsv";
        m.major_version = 1;
        m.minor_version = 0;
        m.menu = "Image";
        m.submenu = "Color";
        m.menu_item = "Adjust HSV";
		m.help = "Adjusts the hue, saturation and value of the input image";
        m.create = &create_adjust_hsv_node;
        inited = true;
    }

    return m;
}

static bool registered = node_factory_t::instance().register_node( adjust_hsv_node_t::adjust_hsv_node_metaclass());

} // namespace
} // namespace
