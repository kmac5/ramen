// Copyright (c) 2012 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_NODES_GROUP_NODE_HPP
#define RAMEN_NODES_GROUP_NODE_HPP

#include<ramen/nodes/composite_node.hpp>

namespace ramen
{

/**
\ingroup nodes
\brief Group node.
*/
class group_node_t : public composite_node_t
{
public:

    /// Constructor.
    group_node_t();

    /// Destructor.
	virtual ~group_node_t();

	/// Dispatch function for visitor pattern.
	//virtual void accept( node_visitor& v);

protected:

	group_node_t( const group_node_t& other);
	void operator=( const group_node_t&);
};

} // namespace

#endif
