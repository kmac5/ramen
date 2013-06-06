// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/node_plug.hpp>

#include<algorithm>

#include<ramen/nodes/node.hpp>

namespace ramen
{
namespace
{

struct match_output_connection
{
    match_output_connection( ramen::node_t *node, const adobe::name_t& id)
    {
        node_ = node;
        id_ = id;
    }

    bool operator()( const node_output_plug_t::connection_t& c) const
    {
        if( boost::get<0>( c) == node_ && boost::get<1>( c) == id_)
            return true;

        return false;
    }

    ramen::node_t *node_;
    adobe::name_t id_;
};

struct match_output_connection_by_port
{
    match_output_connection_by_port( ramen::node_t *node, int num )
    {
        node_ = node;
        num_ = num;
    }

    bool operator()( const node_output_plug_t::connection_t& c) const
    {
        if( boost::get<0>( c) == node_ && boost::get<2>( c) == num_)
            return true;

        return false;
    }

    ramen::node_t *node_;
    int num_;
};

} // unnamed

node_output_plug_t::node_output_plug_t( ramen::node_t *parent, const std::string& id,
										const Imath::Color3c& color,
										const std::string& tooltip) : node_plug_t( id, color, tooltip),
																	dependency::output_node_t(),
																	parent_( parent)
{
	RAMEN_ASSERT( parent_);
}

node_output_plug_t::node_output_plug_t( const node_output_plug_t& other) : node_plug_t( other)
{
    parent_ = 0;
}

node_output_plug_t::~node_output_plug_t() {}

void node_output_plug_t::add_output( ramen::node_t *n, const adobe::name_t& plug)
{
	RAMEN_ASSERT( std::find_if( connections_.begin(), connections_.end(), match_output_connection( n, plug)) == connections_.end());

    int port = n->find_input( plug);
    RAMEN_ASSERT( port >= 0);

    connections_.push_back( boost::tuples::make_tuple( n, plug, port));
}

void node_output_plug_t::add_output( ramen::node_t *n, int port)
{
	RAMEN_ASSERT( std::find_if( connections_.begin(), connections_.end(), match_output_connection_by_port( n, port)) == connections_.end());
    connections_.push_back( boost::tuples::make_tuple( n, n->input_plugs()[port].id(), port));
}

void node_output_plug_t::remove_output( ramen::node_t *n, const adobe::name_t& plug)
{
	iterator it( std::find_if( connections_.begin(), connections_.end(), match_output_connection( n, plug)));

	if( it != connections_.end())
		connections_.erase( it);
}

void node_output_plug_t::remove_output( ramen::node_t *n, int port)
{
	iterator it( std::find_if( connections_.begin(), connections_.end(), match_output_connection_by_port( n, port)));

	if( it != connections_.end())
		connections_.erase( it);
}

node_output_plug_t *node_output_plug_t::do_clone() const
{
    return new node_output_plug_t( *this);
}

node_output_plug_t *new_clone( const node_output_plug_t& other) { return other.clone();}

} // namespace
