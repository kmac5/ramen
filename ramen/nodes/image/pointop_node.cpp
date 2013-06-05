// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/pointop_node.hpp>

namespace ramen
{
namespace image
{

pointop_node_t::pointop_node_t() : image_node_t()
{
    add_input_plug( "front", false, ui::palette_t::instance().color("front plug"), "Front");
    add_output_plug();
}

pointop_node_t::pointop_node_t( const pointop_node_t& other) : image_node_t( other) {}

void pointop_node_t::do_process( const render::context_t& context)
{
    Imath::Box2i area( ImathExt::intersect( input_as<image_node_t>()->defined(), defined()));

    if( area.isEmpty())
		return;

    do_process( input_as<image_node_t>()->const_subimage_view( area), subimage_view( area), context);
}

} // namespace
} // namespace
