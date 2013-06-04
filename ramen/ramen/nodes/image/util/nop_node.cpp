// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/util/nop_node.hpp>

#include<ramen/params/float_param.hpp>

namespace ramen
{
namespace image
{

nop_node_t::nop_node_t() : image_node_t()
{
    set_name( "nop");
    add_input_plug( "front", false, ui::palette_t::instance().color("front plug"), "Front");
    add_output_plug();
}

// factory
node_t *create_nop_node() { return new nop_node_t();}

const node_metaclass_t *nop_node_t::metaclass() const { return &nop_node_metaclass();}

const node_metaclass_t& nop_node_t::nop_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.nop";
        info.major_version = 1;
        info.minor_version = 0;
        info.menu = "Image";
        info.submenu = "Util";
        info.menu_item = "Nop";
		info.help = "No operation (Null)";
        info.create = &create_nop_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( nop_node_t::nop_node_metaclass());

} // namespace
} // namespace
