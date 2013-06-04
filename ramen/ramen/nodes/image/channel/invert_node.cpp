// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/nodes/image/channel/invert_node.hpp>

#include<ramen/params/popup_param.hpp>

#include<ramen/image/processing.hpp>

namespace ramen
{
namespace image
{

invert_node_t::invert_node_t() : pointop_node_t() { set_name( "invert");}

void invert_node_t::do_create_params()
{
    std::auto_ptr<popup_param_t> p( new popup_param_t( "Channels"));
    p->set_id( "channels");
    p->add_menu_item( "RGB");
    p->add_menu_item( "Alpha");
    add_param( p);
}

void invert_node_t::do_calc_bounds( const render::context_t& context)
{
    int alpha = get_value<int>( param( "channels"));

    if( alpha)
		set_bounds( format());
    else
		set_bounds( input_as<image_node_t>()->bounds());
}

void invert_node_t::do_process( const render::context_t& context)
{
    Imath::Box2i area( Imath::intersect( input_as<image_node_t>()->defined(), defined()));

    if( area.isEmpty())
	return;

    if( get_value<int>( param( "channels")))
	boost::gil::fill_pixels( image_view(), image::pixel_t( 0, 0, 0, 1));

    do_process( input_as<image_node_t>()->const_subimage_view( area), subimage_view( area), context);
}

void invert_node_t::do_process( const image::const_image_view_t& src, const image::image_view_t& dst, const render::context_t& context)
{
    int alpha = get_value<int>( param( "channels"));

    if( alpha)
	image::invert_alpha( src, dst);
    else
	image::invert_rgb( src, dst);
}

// factory
node_t *create_invert_node() { return new invert_node_t();}

const node_metaclass_t *invert_node_t::metaclass() const { return &invert_node_metaclass();}

const node_metaclass_t& invert_node_t::invert_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t m;

    if( !inited)
    {
        m.id = "image.builtin.invert";
        m.major_version = 1;
        m.minor_version = 0;
        m.menu = "Image";
        m.submenu = "Channel";
        m.menu_item = "Invert";
        m.help = "Inverts the RGB or alpha channel of the input";
        m.create = &create_invert_node;
        inited = true;
    }

    return m;
}

static bool registered = node_factory_t::instance().register_node( invert_node_t::invert_node_metaclass());

} // namespace
} // namespace
