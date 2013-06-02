// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/graph_algorithm.hpp>

namespace ramen
{
namespace detail
{

void set_inputs_color( node_t& n, graph_color_t c)
{
    n.set_graph_color( c);
	
    for( unsigned int i=0;i<n.num_inputs();++i)
    {
        if( n.input(i) != 0)
            set_inputs_color( *n.input(i), c);
    }
}

void set_outputs_color( node_t& n, graph_color_t c)
{
    n.set_graph_color( c);

    for( unsigned int i=0;i<n.num_outputs();++i)
        set_outputs_color( *n.output(i), c);
}

struct search_node
{
    search_node( node_t& n) : n_( &n), found( false) {}

    void operator()( node_t& node)
    {
        if( n_ == &node)
            found = true;
    }

    node_t *n_;
    bool found;
};

} // detail


bool node_depends_on_node( node_t& node, node_t& other)
{
    // a node depends on other node if you can reach node
    // starting from other node and following the outputs.
    // In other words, if there is a path from other to source.
    detail::search_node fun( node);

    // it would be so nice if boost::ref defined operator()...
    // at least we can wrap the ref inside boost bind.
    breadth_first_outputs_search( other, boost::bind<void>( boost::ref( fun), _1));
    return fun.found;
}

} // namespace
