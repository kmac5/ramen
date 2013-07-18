// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/node_visitor.hpp>

#include<ramen/nodes/image_node.hpp>

namespace ramen
{

node_visitor::~node_visitor() {}

generic_node_visitor::~generic_node_visitor() {}

void generic_node_visitor::visit( image_node_t *n)
{
    visit( static_cast<node_t*>( n));
}

} // ramen
