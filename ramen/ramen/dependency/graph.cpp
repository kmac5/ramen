// Copyright (c) 2012 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/dependency/graph.hpp>

#include<boost/graph/visitors.hpp>
#include<boost/graph/depth_first_search.hpp>
#include<boost/graph/topological_sort.hpp>

#include<ramen/assert.hpp>

namespace ramen
{
namespace dependency
{
namespace
{

struct cycle_detector : public boost::dfs_visitor<>
{
	cycle_detector() : has_cycle( false) {}

	template <class Edge, class Graph>
	void back_edge( Edge, Graph&) { has_cycle = true;}

	bool has_cycle;
};

} // unnamed

graph_t::graph_t() : valid_order_( true) {}
graph_t::~graph_t() {}

void graph_t::add_node( node_t *v)
{
	RAMEN_ASSERT( !has_node( v));

	vertex_desc_type vd = boost::add_vertex( graph_);
	graph_[vd].node = v;
	node_to_desc_[v] = vd;
	valid_order_ = false;
}

void graph_t::remove_node( node_t *v)
{
	RAMEN_ASSERT( has_node( v));

	vertex_desc_type vd = node_to_desc_[v];
	boost::clear_vertex( vd, graph_);
	boost::remove_vertex( vd, graph_);
	node_to_desc_.erase( v);
	valid_order_ = false;
}

bool graph_t::has_node( node_t *v) const
{
	return node_to_desc_.find( v) != node_to_desc_.end();
}

void graph_t::add_dependency( node_t *s, node_t *d)
{
	RAMEN_ASSERT( has_node( s) && has_node( d));
	RAMEN_ASSERT( s != d);

	boost::add_edge( node_to_desc_[s], node_to_desc_[d], graph_);

	cycle_detector v;
	boost::depth_first_search( graph_, boost::visitor( v));

	if( v.has_cycle)
	{
		remove_dependency( s, d);
		throw cycle_error();
	}

	valid_order_ = false;
}

void graph_t::remove_dependency( node_t *s, node_t *d)
{
	RAMEN_ASSERT( s != d);

	if( has_node( s) && has_node( d))
	{
		vertex_desc_type vs = node_to_desc_[s];
		vertex_desc_type vd = node_to_desc_[d];
		boost::remove_edge( vs, vd, graph_);
		valid_order_ = false;
	}
}

void graph_t::clear_all_dirty()
{
	for( std::map<node_t*, vertex_desc_type>::iterator it = node_to_desc_.begin(), ie = node_to_desc_.end(); it != ie; ++it)
		it->first->set_dirty( false);
}

void graph_t::set_node_and_dependencies_dirty( node_t *n)
{
	n->set_dirty( true);
	vertex_desc_type v = node_to_desc_[n];

	boost::graph_traits<graph_type>::out_edge_iterator it, ie;
	for( boost::tie( it, ie) = boost::out_edges(v, graph_); it != ie; ++it)
	{
		vertex_desc_type dst = boost::target( *it, graph_);
		node_t *nn = graph_[dst].node;
		set_node_and_dependencies_dirty( nn);
	}
}

void graph_t::notify_all_dirty()
{
	if( !valid_order_)
	{
		sorted_vertices_.clear();
		boost::topological_sort( graph_, std::back_inserter( sorted_vertices_));
		valid_order_ = true;
	}

	for( int i = 0, e = sorted_vertices_.size(); i < e; ++i)
	{
		vertex_desc_type n = sorted_vertices_[i];
		node_t *nn = graph_[n].node;
		nn->notify();
	}
}

} // dependency
} // ramen
