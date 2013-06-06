// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/nodes/node_visitor.hpp>

#include<ramen/nodes/composite_node.hpp>
#include<ramen/nodes/image_node.hpp>

namespace ramen
{

node_visitor::~node_visitor() {}

generic_node_visitor::~generic_node_visitor() {}

void generic_node_visitor::visit( composite_node_t *n)
{
    visit( static_cast<node_t*>( n));
}

void generic_node_visitor::visit( image_node_t *n)
{
    visit( static_cast<node_t*>( n));
}

} // namespace
