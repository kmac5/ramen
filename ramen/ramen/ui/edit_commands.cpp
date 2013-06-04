// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/python/python.hpp>

#include<ramen/ui/edit_commands.hpp>

#include<boost/bind.hpp>
#include<boost/foreach.hpp>
#include<boost/range/algorithm/for_each.hpp>

#include<ramen/app/application.hpp>
#include<ramen/app/document.hpp>

#include<ramen/nodes/graph_algorithm.hpp>

namespace ramen
{
namespace undo
{

extract_command_t::extract_command_t() : command_t( "Extract") {}

void extract_command_t::add_edge_to_remove( const edge_t& e)	{ edges_to_remove_.push_back( e);}
void extract_command_t::add_edge_to_add( const edge_t& e)		{ edges_to_add_.push_back( e);}

void extract_command_t::add_dependent_node( node_t *n) { dependents_.insert( n);}

bool extract_command_t::edge_less( const edge_t& a, const edge_t& b)
{
	if( a.dst < b.dst)
		return true;
	else
	{
		if( a.dst > b.dst)
			return false;
		else
			return a.port < b.port;
	}
}

bool extract_command_t::edge_compare( const edge_t& a, const edge_t& b)
{
	return a.dst == b.dst && a.port == b.port;
}

void extract_command_t::add_candidate_edge( const edge_t& e, node_t *src, std::vector<edge_t>& edges)
{
	if( e.src->selected() && !e.dst->selected())
		edges.push_back( edge_t( src, e.dst, e.port));
}

void extract_command_t::undo()
{
    composition_t *comp = &app().document().composition();
    boost::range::for_each( edges_to_add_, boost::bind( &composition_t::remove_edge, comp, _1, true));
    boost::range::for_each( edges_to_remove_, boost::bind( &composition_t::add_edge, comp, _1, true));
    breadth_first_multiple_outputs_search( dependents_, boost::bind( &node_t::notify, _1));
    command_t::undo();
}

void extract_command_t::redo()
{
    composition_t *comp = &app().document().composition();
    boost::range::for_each( edges_to_remove_, boost::bind( &composition_t::remove_edge, comp, _1, true));
    boost::range::for_each( edges_to_add_, boost::bind( &composition_t::add_edge, comp, _1, true));
    breadth_first_multiple_outputs_search( dependents_, boost::bind( &node_t::notify, _1));
    command_t::redo();
}

delete_command_t::delete_command_t() { set_name( "Delete");}

void delete_command_t::add_node( node_t *n)
{
    nodes_.push_back( n);
}

void delete_command_t::undo()
{
    composition_t *comp = &app().document().composition();
	
    boost::range::for_each( edges_to_add_, boost::bind( &composition_t::remove_edge, comp, _1, true));
	
    while( !node_storage_.empty())
    {
        std::auto_ptr<node_t> ptr( node_storage_.pop_back().release());
        app().document().composition().add_node( ptr);
    }

    boost::range::for_each( edges_to_remove_, boost::bind( &composition_t::add_edge, comp, _1, true));
    breadth_first_multiple_outputs_search( dependents_, boost::bind( &node_t::notify, _1));
    command_t::undo();
}

void delete_command_t::redo()
{
    for( std::vector<node_t*>::const_iterator it( nodes_.begin()); it != nodes_.end(); ++it)
    {
        std::auto_ptr<node_t> ptr( app().document().composition().release_node( *it));
        node_storage_.push_back( ptr);
    }

    composition_t *comp = &app().document().composition();
    boost::range::for_each( edges_to_remove_, boost::bind( &composition_t::remove_edge, comp, _1, true));
    boost::range::for_each( edges_to_add_, boost::bind( &composition_t::add_edge, comp, _1, true));
	
    breadth_first_multiple_outputs_search( dependents_, boost::bind( &node_t::notify, _1));
    command_t::redo();
}

duplicate_command_t::duplicate_command_t() : command_t( "Duplicate") {}

void duplicate_command_t::add_node( std::auto_ptr<node_t> n)
{
    nodes_.push_back( n.get());
    node_storage_.push_back( n);
}

void duplicate_command_t::add_edge( const edge_t& e) { edges_.push_back( e);}

void duplicate_command_t::undo()
{
    for( std::vector<node_t*>::const_iterator it( nodes_.begin()); it != nodes_.end(); ++it)
    {
        std::auto_ptr<node_t> ptr( app().document().composition().release_node( *it));
        node_storage_.push_back( ptr);
    }

    composition_t *comp = &app().document().composition();
    boost::range::for_each( edges_, boost::bind( &composition_t::remove_edge, comp, _1, true));
    command_t::undo();
}

void duplicate_command_t::redo()
{
    while( !node_storage_.empty())
    {
        std::auto_ptr<node_t> ptr( node_storage_.pop_back().release());
        app().document().composition().add_node( ptr);
    }

    composition_t *comp = &app().document().composition();
    boost::range::for_each( edges_, boost::bind( &composition_t::add_edge, comp, _1, true));
    command_t::redo();
}

ignore_nodes_command_t::ignore_nodes_command_t() : command_t( "Ignore") {}

void ignore_nodes_command_t::add_node( node_t *n) { nodes_.push_back( n);}

void ignore_nodes_command_t::undo()
{
    for( std::vector<node_t*>::const_iterator it( nodes_.begin()); it != nodes_.end(); ++it)
        (*it)->set_ignored( !(*it)->ignored());

    breadth_first_multiple_outputs_search( nodes_, boost::bind( &node_t::notify, _1));
    command_t::undo();
}

void ignore_nodes_command_t::redo()
{
    for( std::vector<node_t*>::const_iterator it( nodes_.begin()); it != nodes_.end(); ++it)
        (*it)->set_ignored( !(*it)->ignored());

    breadth_first_multiple_outputs_search( nodes_, boost::bind( &node_t::notify, _1));
    command_t::redo();
}

} // namespace
} // namespace
