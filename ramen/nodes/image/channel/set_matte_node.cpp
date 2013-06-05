// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/channel/set_matte_node.hpp>

#include<ramen/params/bool_param.hpp>
#include<ramen/image/processing.hpp>

#include<ramen/gil/extension/algorithm/tbb/tbb.hpp>

namespace ramen
{
namespace image
{
namespace
{

struct copy_rgb_and_clear_alpha
{
    image::pixel_t operator()( const image::pixel_t& src) const
    {
		image::pixel_t dst( src);
		dst[3] = 0;
		return dst;
    }
};

} // detail

set_matte_node_t::set_matte_node_t() : image_node_t()
{
    set_name( "set_matte");
    add_input_plug( "front", false, ui::palette_t::instance().color( "front plug"), "Front");
    add_input_plug( "matte", false, ui::palette_t::instance().color( "matte plug"), "Matte");
    add_output_plug();
}

void set_matte_node_t::do_create_params()
{
    std::auto_ptr<bool_param_t> q( new bool_param_t( "Premultiply"));
    q->set_id( "premultiply");
    q->set_default_value( true);
    add_param( q);
}

void set_matte_node_t::do_calc_bounds( const render::context_t& context)
{
    if( get_value<bool>( param( "premultiply")))
		set_bounds( ImathExt::intersect( input_as<image_node_t>( 0)->bounds(), input_as<image_node_t>( 1)->bounds()));
    else
		set_bounds( input_as<image_node_t>( 0)->bounds());
}

void set_matte_node_t::do_process( const render::context_t& context)
{
    if( defined().isEmpty())
	return;

    boost::gil::tbb_transform_pixels( input_as<image_node_t>( 0)->const_subimage_view( defined()), image_view(),
					  copy_rgb_and_clear_alpha());

    Imath::Box2i area = ImathExt::intersect( defined(), input_as<image_node_t>( 1)->defined());

    if( !area.isEmpty())
	boost::gil::tbb_copy_pixels( boost::gil::nth_channel_view( input_as<image_node_t>( 1)->const_subimage_view( area), 3),
					    boost::gil::nth_channel_view( subimage_view( area), 3));

    if( get_value<bool>( param( "premultiply")))
	image::premultiply( image_view(), image_view());
}

// factory
node_t *create_set_matte_node() { return new set_matte_node_t();}

const node_metaclass_t *set_matte_node_t::metaclass() const { return &set_matte_node_metaclass();}

const node_metaclass_t& set_matte_node_t::set_matte_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t m;

    if( !inited)
    {
        m.id = "image.builtin.set_matte";
        m.major_version = 1;
        m.minor_version = 0;
        m.menu = "Image";
        m.submenu = "Channel";
        m.menu_item = "Set Matte";
        m.help = "Replaces the alpha channel of the first input by the alpha channel of the second input.";
        m.create = &create_set_matte_node;
        inited = true;
    }

    return m;
}

static bool registered = node_factory_t::instance().register_node( set_matte_node_t::set_matte_node_metaclass());

} // namespace
} // namespace
