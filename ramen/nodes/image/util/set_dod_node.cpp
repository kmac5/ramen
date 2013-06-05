// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/util/set_dod_node.hpp>

#include<ramen/params/float_param.hpp>

namespace ramen
{
namespace image
{

set_dod_node_t::set_dod_node_t() : image_node_t()
{
    set_name( "dod");
    add_input_plug( "front", false, ui::palette_t::instance().color("front plug"), "Front");
    add_output_plug();
}

void set_dod_node_t::do_create_params()
{
    std::auto_ptr<float_param_t> p( new float_param_t( "Top"));
    p->set_id( "top");
    p->set_range( 0, 1);
    p->set_numeric_type( numeric_param_t::relative_size_y, 0);
    p->set_default_value( 0);
    add_param( p);

    p.reset( new float_param_t( "Left"));
    p->set_id( "left");
    p->set_range( 0, 1);
    p->set_numeric_type( numeric_param_t::relative_size_x, 0);
    p->set_default_value( 0);
    add_param( p);

    p.reset( new float_param_t( "Bottom"));
    p->set_id( "bottom");
    p->set_range( 0, 1);
    p->set_numeric_type( numeric_param_t::relative_size_y, 0);
    p->set_default_value( 0);
    add_param( p);

    p.reset( new float_param_t( "Right"));
    p->set_id( "right");
    p->set_range( 0, 1);
    p->set_numeric_type( numeric_param_t::relative_size_x, 0);
    p->set_default_value( 0);
    add_param( p);
}

void set_dod_node_t::do_calc_bounds( const render::context_t& context)
{
    Imath::Box2i bounds( ImathExt::intersect( input_as<image_node_t>()->bounds(), dod_area()));
    set_bounds( bounds);
}

void set_dod_node_t::do_calc_inputs_interest( const render::context_t& context)
{
    Imath::Box2i roi = ImathExt::intersect( interest(), dod_area());
    input_as<image_node_t>()->add_interest( roi);
}

void set_dod_node_t::do_process( const render::context_t& context)
{
    Imath::Box2i area = ImathExt::intersect( input_as<image_node_t>()->defined(), defined());

    if( area.isEmpty())
		return;

    boost::gil::copy_pixels( input_as<image_node_t>()->const_subimage_view( area), subimage_view( area));
}

Imath::Box2i set_dod_node_t::dod_area() const
{
    Imath::Box2i box( format());
    int crop_t = get_absolute_value<float>( param( "top"));
    int crop_l = get_absolute_value<float>( param( "left"));
    int crop_b = get_absolute_value<float>( param( "bottom"));
    int crop_r = get_absolute_value<float>( param( "right"));
	return Imath::Box2i( Imath::V2i( box.min.x + crop_l, box.min.y + crop_t), Imath::V2i( box.max.x - crop_r, box.max.y - crop_b));
}

// factory
node_t *create_set_dod_node() { return new set_dod_node_t();}

const node_metaclass_t *set_dod_node_t::metaclass() const { return &set_dod_node_metaclass();}

const node_metaclass_t& set_dod_node_t::set_dod_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.setdod";
        info.major_version = 1;
        info.minor_version = 0;
        info.menu = "Image";
        info.submenu = "Util";
        info.menu_item = "Set DoD";
		info.help = "Limits processing to the specified area";
        info.create = &create_set_dod_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( set_dod_node_t::set_dod_node_metaclass());

} // namespace
} // namespace
