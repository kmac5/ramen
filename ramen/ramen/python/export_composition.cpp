// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/python/python.hpp>

#include<boost/foreach.hpp>
#include<boost/python.hpp>
#include<boost/python/ptr.hpp>
namespace bpy = boost::python;

#include<ramen/app/document.hpp>

#include<ramen/python/read_only_vector_proxy.hpp>

namespace ramen
{
namespace python
{
namespace
{
/*
// edges
node_ptr_t edge_get_source( const edge_t& e)	{ return node_ptr_t( e.src);}
node_ptr_t edge_get_dest( const edge_t& e)	{ return node_ptr_t( e.dst);}
int edge_get_port( const edge_t& e)			{ return e.port;}

// comp
composition_t& current_comp()
{
	return app().document().composition();
}

bpy::list make_node_list( composition_t& c)
{
	bpy::list l;
	
	BOOST_FOREACH( node_ptr_t& n, c.nodes())
		l.append( n);
	
	return l;
}

read_only_vector_proxy_t<node_ptr_t> ro_node_list( const composition_t& c)
{
    return read_only_vector_proxy_t<node_ptr_t>( c.nodes());
}

read_only_vector_proxy_t<edge_t> ro_edge_list( const composition_t& c)
{
    return read_only_vector_proxy_t<edge_t>( c.edges());
}

bpy::list make_selected_node_list( composition_t& c)
{
	bpy::list l;
	
	BOOST_FOREACH( node_ptr_t& n, c.nodes())
	{
		if( n->selected())
			l.append( n);
	}
	
	return l;
}

node_ptr_t find_node( composition_t& c, const std::string& name)
{ 
	node_t *n = c.find_node( name);
	return node_ptr_t( n);
}

node_ptr_t selected_node( composition_t& c)
{ 
	node_t *n = c.selected_node();
	return node_ptr_t( n);
}

bool can_connect( composition_t& c, node_ptr_t& src, node_ptr_t& dst, int port)
{
	return c.can_connect( src.get(), dst.get(), port);
}

bool connect( composition_t& c, node_ptr_t& src, node_ptr_t& dst, int port)
{
	c.connect( src.get(), dst.get(), port);
}

bool disconnect( composition_t& c, node_ptr_t& src, node_ptr_t& dst, int port)
{
	c.disconnect( src.get(), dst.get(), port);
}
*/
} // namespace

void export_composition()
{
    /*
	// export a vector of nodes
	read_only_vector_proxy_t<node_ptr_t>::bind( "node_vector_t");

	// edges
    bpy::class_<edge_t>( "edge_t", bpy::no_init)
		.add_property( "src", edge_get_source)
		.add_property( "dst", edge_get_dest)
		.add_property( "port", edge_get_port)
        ;
	
	read_only_vector_proxy_t<edge_t>::bind( "edge_vector_t");
	
	bpy::class_<composition_t, boost::noncopyable>( "composition_t")
			.def( "nodes", ro_node_list)
			.def( "edges", ro_edge_list)

			.def( "selected_nodes", make_selected_node_list)
			.def( "find_node", find_node)
			
			.def( "selected_node", selected_node)
			.def( "deselect_all", &composition_t::deselect_all)
			
			.def( "start_frame", &composition_t::start_frame)
			.def( "set_start_frame", &composition_t::set_start_frame)
			
			.def( "end_frame" , &composition_t::end_frame)
			.def( "set_end_frame" , &composition_t::set_end_frame)
			
			.def( "frame", &composition_t::frame)
			.def( "set_frame", &composition_t::set_frame)

			.def( "autokey", &composition_t::autokey)
			.def( "set_autokey", &composition_t::set_autokey)
			
			.def( "can_connect", can_connect)
			.def( "connect", connect)
			.def( "disconnect", disconnect)
			;
	
	bpy::def( "composition", current_comp, bpy::return_value_policy<bpy::reference_existing_object>());
    */
}

} // python
} // ramen
