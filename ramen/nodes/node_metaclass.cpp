// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/python/python.hpp>

#include<ramen/nodes/node_metaclass.hpp>

#include<ramen/nodes/node.hpp>

namespace ramen
{

node_metaclass_t::node_metaclass_t()
{
    ui_visible = true;
    help = 0;
	first_time_ = true;
}

} // namespace
