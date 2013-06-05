// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/transform/flip_node.hpp>

#include<OpenEXR/ImathMatrix.h>

#include<ramen/ImathExt/ImathBoxAlgo.h>

#include<ramen/params/popup_param.hpp>

#include<ramen/image/processing.hpp>

namespace ramen
{
namespace image
{

flip_node_t::flip_node_t() : image_node_t()
{
    set_name("flip");
    add_input_plug( "front", false, ui::palette_t::instance().color("front plug"), "Front");
    add_output_plug();
}

void flip_node_t::do_create_params()
{
    std::auto_ptr<popup_param_t> p( new popup_param_t( "Mode"));
    p->set_id( "mode");
    p->menu_items() = boost::assign::list_of( "Horizontal")( "Vertical")( "Both");
    add_param( p);
}

void flip_node_t::calc_transform_matrix()
{
    float cx = input_as<image_node_t>()->format().center().x;
    float cy = input_as<image_node_t>()->format().center().y;

    float sx, sy;

    switch( get_value<int>( param( "mode")))
    {
    case 0:
        sx = -1.0f;
        sy =  1.0f;
    break;

    case 1:
        sx =  1.0f;
        sy = -1.0f;
    break;

    case 2:
        sx = -1.0f;
        sy = -1.0f;
    break;
    }

    xform_ = Imath::M33d().setTranslation( Imath::V2d( -cx, -cy)) *
             Imath::M33d().setScale( Imath::V2d( sx, sy)) *
             Imath::M33d().setTranslation( Imath::V2d( cx, cy));

    inv_xform_ = xform_.inverse();
}

void flip_node_t::do_calc_bounds( const render::context_t& context)
{
    calc_transform_matrix();
    Imath::Box2i xfbounds( input_as<image_node_t>()->bounds());
    xfbounds = ImathExt::transform( xfbounds, xform_);
    set_bounds( xfbounds);
}

void flip_node_t::do_calc_inputs_interest( const render::context_t& context)
{
    Imath::Box2i roi( ImathExt::transform( interest(), inv_xform_));
    input_as<image_node_t>()->add_interest( roi);
}

void flip_node_t::do_process( const render::context_t& context)
{
    if( defined().isEmpty())
        return;

    calc_transform_matrix();
    image::affine_warp_nearest( input_as<image_node_t>()->defined(), input_as<image_node_t>()->const_image_view(),
                                defined(), image_view(), xform_, inv_xform_);
}

// factory
node_t *create_flip_node() { return new flip_node_t();}

const node_metaclass_t *flip_node_t::metaclass() const { return &flip_node_metaclass();}

const node_metaclass_t& flip_node_t::flip_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.flip";
        info.major_version = 1;
        info.minor_version = 0;
        info.menu = "Image";
        info.submenu = "Transform";
        info.menu_item = "Flip";
        info.create = &create_flip_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( flip_node_t::flip_node_metaclass());

} // namespace
} // namespace
