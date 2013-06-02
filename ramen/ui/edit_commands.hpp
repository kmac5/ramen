// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_EDIT_MENU_COMMANDS_HPP
#define RAMEN_EDIT_MENU_COMMANDS_HPP

#include<ramen/undo/command.hpp>

#include<set>

#include<boost/ptr_container/ptr_vector.hpp>

#include<ramen/app/document.hpp>

namespace ramen
{
namespace undo
{

struct extract_command_t : public command_t
{
public:

	extract_command_t();

    void add_edge_to_remove( const edge_t& e);
    void add_edge_to_add( const edge_t& e);

    void add_dependent_node( node_t *n);
	
    virtual void undo();
    virtual void redo();

	// util
	static bool edge_less( const edge_t& a, const edge_t& b);
	static bool edge_compare( const edge_t& a, const edge_t& b);
	
	static void add_candidate_edge( const edge_t& e, node_t *src, std::vector<edge_t>& edges);
	
protected:

    std::vector<edge_t> edges_to_remove_;
    std::vector<edge_t> edges_to_add_;

    std::set<node_t*> dependents_;
};

struct delete_command_t : public extract_command_t
{
    delete_command_t();
	
    void add_node( node_t *n);

    virtual void undo();
    virtual void redo();

private:

    std::vector<node_t*> nodes_;
    boost::ptr_vector<node_t> node_storage_;
};

struct duplicate_command_t : public command_t
{
    duplicate_command_t();

    void add_node( std::auto_ptr<node_t> n);
    void add_edge( const edge_t& e);

    virtual void undo();
    virtual void redo();

private:

    std::vector<node_t*> nodes_;
    std::vector<edge_t> edges_;
    boost::ptr_vector<node_t> node_storage_;
};

struct ignore_nodes_command_t : public command_t
{
    ignore_nodes_command_t();

    void add_node( node_t *n);

    virtual void undo();
    virtual void redo();

private:

    std::vector<node_t*> nodes_;
};

} // namespace
} // namespace

#endif
