// Copyright (c) 2012 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_DEPENDENCY_GRAPH_HPP
#define RAMEN_DEPENDENCY_GRAPH_HPP

#include<ramen/config.hpp>

#include<ramen/dependency/graph_fwd.hpp>

#include<vector>
#include<map>

#include<boost/graph/adjacency_list.hpp>

#include<ramen/dependency/node.hpp>
#include<ramen/dependency/exceptions.hpp>

namespace ramen
{
namespace dependency
{

/*!
\ingroup depgraph
\brief Ramen's dependency graph.
*/
class RAMEN_API graph_t
{
public:

	graph_t();
	~graph_t();

	///	Adds a node to the dependency graph.
	void add_node( node_t *v);

	/// Removes a node from the dependency graph.
	void remove_node( node_t *v);

	/// Returns true if a node is in the dependency graph.
	bool has_node( node_t *v) const;

	/*!
		Adds a dependency between nodes. s depends on d.
		\param s Source node
		\param d Destination node
		\exception ramen::dependency::cycle_error thrown if adding the dependency creates a cycle in the graph.
	*/
	void add_dependency( node_t *s, node_t *d);

	/*!
		Removes a dependency between nodes. s depends on d.
		\param s Source node
		\param d Destination node
	*/
	void remove_dependency( node_t *s, node_t *d);

	/// Applies a function object to all dependency nodes in the graph.
	template<class Fun>
	void for_each_node( Fun f)
	{
		for( std::map<node_t*, vertex_desc_type>::iterator it = node_to_desc_.begin(), ie = node_to_desc_.end(); it != ie; ++it)
			f( it->first);
	}

	/// Clears all dirty flags.
	void clear_all_dirty();

	/// Dirties a node and all the nodes that depends on it.
	void set_node_and_dependencies_dirty( node_t *n);

    /// Calls notify on all nodes that are dirty.
	void notify_all_dirty();

private:

	struct node_info_t
	{
		node_t *node;
	};

	typedef boost::adjacency_list<	boost::listS,
									boost::listS,
									boost::directedS,
									boost::property<boost::vertex_index_t, int, node_info_t> > graph_type;

	typedef boost::graph_traits<graph_type>::vertex_descriptor	vertex_desc_type;
	typedef boost::graph_traits<graph_type>::edge_descriptor	edge_desc_type;

	graph_type graph_;

	typedef std::map<node_t *, vertex_desc_type> node_to_desc_map_type;
	node_to_desc_map_type node_to_desc_;

	bool valid_order_;
	std::vector<vertex_desc_type> sorted_vertices_;
};

} // dependency
} // ramen

#endif
