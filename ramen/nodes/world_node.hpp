// Copyright (c) 2012 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_NODES_WORLD_NODE_HPP
#define RAMEN_NODES_WORLD_NODE_HPP

#include<ramen/nodes/composite_node.hpp>

namespace ramen
{

/**
\ingroup nodes
\brief A top level node that contains other nodes.
*/
class world_node_t : public composite_node_t
{
public:

	world_node_t();
	virtual ~world_node_t();

    boost::signals2::signal<void ( node_t*)> node_added;
    boost::signals2::signal<void ( node_t*)> node_removed;

protected:

	world_node_t( const world_node_t& other);
	void operator=( const world_node_t&);
};

} // ramen

#endif
