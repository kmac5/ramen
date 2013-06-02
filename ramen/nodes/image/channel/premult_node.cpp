// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/nodes/image/channel/premult_node.hpp>

#include<ramen/params/popup_param.hpp>

#include<ramen/image/processing.hpp>

namespace ramen
{
namespace image
{

premult_node_t::premult_node_t() : pointop_node_t() { set_name("premult");}

void premult_node_t::do_create_params()
{
    std::auto_ptr<popup_param_t> p( new popup_param_t( "Mode"));
    p->set_id( "mode");
    p->add_menu_item( "Premult");
    p->add_menu_item( "Unpremult");
    add_param( std::auto_ptr<param_t>( p.release()));
}

void premult_node_t::do_process( const image::const_image_view_t& src, const image::image_view_t& dst, const render::context_t& context)
{
    int div = get_value<int>( param( "mode"));

    if( div)
	image::unpremultiply( src, dst);
    else
	image::premultiply( src, dst);
}

// factory
node_t *create_premult_node() { return new premult_node_t();}

const node_metaclass_t *premult_node_t::metaclass() const { return &premult_node_metaclass();}

const node_metaclass_t& premult_node_t::premult_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.premult";
        info.major_version = 1;
        info.minor_version = 0;
        info.menu = "Image";
        info.submenu = "Channel";
        info.menu_item = "Premult";
        info.help = "Multiplies or divides the RGB channels by the alpha channel.";
        info.create = &create_premult_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( premult_node_t::premult_node_metaclass());

} // namespace
} // namespace
