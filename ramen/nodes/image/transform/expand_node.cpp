// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/transform/expand_node.hpp>

#include<ramen/gil/extension/algorithm/tbb/tbb.hpp>

#include<ramen/params/float_param.hpp>

namespace ramen
{
namespace image
{

expand_node_t::expand_node_t() : image_node_t()
{
    set_name("expand");
    add_input_plug( "front", false, ui::palette_t::instance().color("front plug"), "Front");
    add_output_plug();
}

void expand_node_t::do_create_params()
{
    std::auto_ptr<float_param_t> p( new float_param_t( "Top"));
    p->set_id( "top");
    p->set_min( 0);
    p->set_numeric_type( numeric_param_t::relative_size_y, 0);
    p->set_default_value( 0);
    p->set_static( true);
    add_param( p);

    p.reset( new float_param_t( "Left"));
    p->set_id( "left");
    p->set_min( 0);
    p->set_numeric_type( numeric_param_t::relative_size_x, 0);
    p->set_default_value( 0);
    p->set_static( true);
    add_param( p);

    p.reset( new float_param_t( "Bottom"));
    p->set_id( "bottom");
    p->set_min( 0);
    p->set_numeric_type( numeric_param_t::relative_size_y, 0);
    p->set_default_value( 0);
    p->set_static( true);
    add_param( p);

    p.reset( new float_param_t( "Right"));
    p->set_id( "right");
    p->set_min( 0);
    p->set_numeric_type( numeric_param_t::relative_size_x, 0);
    p->set_default_value( 0);
    p->set_static( true);
    add_param( p);
}

void expand_node_t::do_calc_format( const render::context_t& context)
{
	image_node_t *in = input_as<image_node_t>();
    Imath::Box2i box( in->format());	
    int exp_t = get_absolute_value<float>( param( "top"));
    int exp_l = get_absolute_value<float>( param( "left"));
    int exp_b = get_absolute_value<float>( param( "bottom"));
    int exp_r = get_absolute_value<float>( param( "right"));
    set_format( Imath::Box2i( Imath::V2i( box.min.x - exp_l, box.min.y - exp_t), Imath::V2i( box.max.x + exp_r, box.max.y + exp_b)));
	set_aspect_ratio( in->aspect_ratio());
	set_proxy_scale( in->proxy_scale());
}

void expand_node_t::do_calc_bounds( const render::context_t& context)
{
    Imath::Box2i bounds( ImathExt::intersect( input_as<image_node_t>()->bounds(), format()));
    set_bounds( bounds);
}

void expand_node_t::do_calc_inputs_interest( const render::context_t& context)
{
    Imath::Box2i roi = ImathExt::intersect( interest(), format());
    input_as<image_node_t>()->add_interest( roi);
}

void expand_node_t::do_process( const render::context_t& context)
{
    Imath::Box2i area = ImathExt::intersect( input_as<image_node_t>()->defined(), defined());

    if( area.isEmpty())
        return;

    boost::gil::copy_pixels( input_as<image_node_t>()->const_subimage_view( area), subimage_view( area));
}

// factory
node_t *create_expand_node() { return new expand_node_t();}

const node_metaclass_t *expand_node_t::metaclass() const { return &expand_node_metaclass();}

const node_metaclass_t& expand_node_t::expand_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.expand";
        info.major_version = 1;
        info.minor_version = 0;
        info.menu = "Image";
        info.submenu = "Transform";
        info.menu_item = "Expand";
        info.create = &create_expand_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( expand_node_t::expand_node_metaclass());

} // namespace
} // namespace
