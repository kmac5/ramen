// Copyright (c) 2012 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/group_node.hpp>

namespace ramen
{

group_node_t::group_node_t() : composite_node_t() {}

group_node_t::group_node_t( const group_node_t& other) : composite_node_t( other) {}

group_node_t::~group_node_t() {}

void group_node_t::accept( node_visitor& v) { v.visit( this);}

} // namespace
