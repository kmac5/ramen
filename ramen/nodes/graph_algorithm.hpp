// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_NODES_GRAPH_ALGORITHM_HPP
#define RAMEN_NODES_GRAPH_ALGORITHM_HPP

#include<boost/range.hpp>
#include<boost/bind.hpp>
#include<boost/ref.hpp>

#include<ramen/nodes/node.hpp>
#include<ramen/nodes/edge.hpp>

namespace ramen
{

namespace detail
{

void set_inputs_color( node_t& n, graph_color_t c);

template<class Range>
void set_multiple_inputs_color( Range& r, graph_color_t c)
{
    typedef typename boost::range_iterator<Range>::type iter;

    for( iter it( boost::begin(r)); it != boost::end(r); ++it)
        set_inputs_color( *(*it), c);
}

void set_outputs_color( node_t& n, graph_color_t c);

template<class Range>
void set_multiple_outputs_color( Range& r, graph_color_t c)
{
    typedef typename boost::range_iterator<Range>::type iter;

    for( iter it( boost::begin(r)); it != boost::end(r); ++it)
        set_outputs_color( *(*it), c);
}

template<class Visitor>
void depth_first_inputs_recursive_search( node_t& n, Visitor f)
{
    for( unsigned int i=0;i<n.num_inputs();++i)
    {
        if( n.input(i) != 0)
            depth_first_inputs_recursive_search( *n.input(i), f);
    }
	
    if( n.graph_color() == black)
    {
        f( n);
        n.set_graph_color( white);
    }
}

template<class Visitor>
void breadth_first_inputs_recursive_search( node_t& n, Visitor f)
{
    if( n.graph_color() == black)
    {
        f( n);
        n.set_graph_color( white);
    }

    for( unsigned int i=0;i<n.num_inputs();++i)
    {
        if( n.input(i) != 0)
            breadth_first_inputs_recursive_search( *n.input(i), f);
    }
}

template<class Visitor>
void depth_first_outputs_recursive_search( node_t& n, Visitor f)
{
    for( unsigned int i=0;i<n.num_outputs();++i)
        depth_first_outputs_recursive_search( *n.output(i), f);
	
    if( n.graph_color() == black)
    {
        f( n);
        n.set_graph_color( white);
    }
}

template<class Visitor>
void breadth_first_outputs_recursive_search( node_t& n, Visitor f)
{
    if( n.graph_color() == black)
    {
        f( n);
        n.set_graph_color( white);
    }

    for( unsigned int i=0;i<n.num_outputs();++i)
        breadth_first_outputs_recursive_search( *n.output(i), f);
}

} // detail

template<class Visitor>
void breadth_first_inputs_search( node_t& first, Visitor f, bool search_first = true)
{
    detail::set_inputs_color( first, black);

    if( !search_first)
        first.set_graph_color( white);

    detail::breadth_first_inputs_recursive_search( first, f);
}

template<class Visitor>
void breadth_first_inputs_apply( node_t& n, Visitor f, bool apply_first = true)
{
    if( apply_first)
        f( n);

    for( unsigned int i=0;i<n.num_inputs();++i)
    {
        if( n.input(i) != 0)
            breadth_first_inputs_apply( *n.input(i), f);
    }
}

template<class Visitor>
void depth_first_inputs_search( node_t& first, Visitor f, bool search_first = true)
{
    detail::set_inputs_color( first, black);

    if( !search_first)
        first.set_graph_color( white);

    detail::depth_first_inputs_recursive_search( first, f);
}

template<class Visitor>
void breadth_first_outputs_search( node_t& first, Visitor f, bool search_first = true)
{
    detail::set_outputs_color( first, black);

    if( !search_first)
        first.set_graph_color( white);

    detail::breadth_first_outputs_recursive_search( first, f);
}

template<class Range, class Visitor>
void breadth_first_multiple_outputs_search( Range& r, Visitor f, bool search_first = true)
{
    typedef typename boost::range_iterator<Range>::type iter;

    detail::set_multiple_outputs_color( r, black);

    if( !search_first)
    {
        for( iter it( boost::begin(r)); it != boost::end(r); ++it)
            (*it)->set_graph_color( white);
    }

    for( iter it( boost::begin(r)); it != boost::end(r); ++it)
        detail::breadth_first_outputs_recursive_search( *(*it), f);
}

template<class Visitor>
void depth_first_outputs_search( node_t& first, Visitor f, bool search_first = true)
{
    detail::set_outputs_color( first, black);

    if( !search_first)
        first.set_graph_color( white);

    detail::depth_first_outputs_recursive_search( first, f);
}

bool node_depends_on_node( node_t& node, node_t& other);

template<class Visitor>
void breadth_first_out_edges_apply( node_t& n, Visitor f)
{
    for( unsigned int i = 0; i < n.num_outputs(); ++i)
	{
		node_t *dst = boost::get<0>( n.output_plug().connections()[i]);
		int port  = boost::get<2>( n.output_plug().connections()[i]);
		edge_t e( &n, dst, port);
		f( e);

		breadth_first_out_edges_apply( *dst, f);
	}
}

} // namespace

#endif
