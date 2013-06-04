// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/nodes/image/color/gamma_node.hpp>

#include<ramen/params/float_param.hpp>
#include<ramen/image/processing.hpp>

namespace ramen
{
namespace image
{

gamma_node_t::gamma_node_t() : pointop_node_t()
{
    set_name("gamma");
}

void gamma_node_t::do_create_params()
{
    std::auto_ptr<float_param_t> p( new float_param_t( "Gamma"));
    p->set_id( "gamma");
    p->set_default_value( 1.0f);
    p->set_range( 0.1f, 20);
    p->set_step( 0.05);
    add_param( p);
}

bool gamma_node_t::do_is_identity() const
{
	return get_value<float>( param( "gamma")) == 1.0f;
}

void gamma_node_t::do_process( const image::const_image_view_t& src, const image::image_view_t& dst, const render::context_t& context)
{
    image::apply_gamma( src, dst, 1.0f / get_value<float>( param( "gamma")));
}

// factory
node_t *create_gamma_node() { return new gamma_node_t();}

const node_metaclass_t *gamma_node_t::metaclass() const { return &gamma_node_metaclass();}

const node_metaclass_t& gamma_node_t::gamma_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.gamma";
        info.major_version = 1;
        info.minor_version = 0;
        info.menu = "Image";
        info.submenu = "Color";
        info.menu_item = "Gamma";
        info.create = &create_gamma_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( gamma_node_t::gamma_node_metaclass());

} // namespace
} // namespace
