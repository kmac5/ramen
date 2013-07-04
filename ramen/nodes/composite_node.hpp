// Copyright (c) 2012 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_NODES_COMPOSITE_NODE_HPP
#define RAMEN_NODES_COMPOSITE_NODE_HPP

#include<ramen/nodes/node.hpp>

#include<memory>
#include<set>

#include<ramen/nodes/node_graph.hpp>
#include<ramen/nodes/node_visitor.hpp>
#include<ramen/nodes/edge.hpp>

#include<ramen/serialization/archive_fwd.hpp>
#include<ramen/serialization/yaml.hpp>

#include<ramen/ui/graph_layout_fwd.hpp>

namespace ramen
{

/**
\ingroup nodes
\brief Abstract base class for nodes that contain other nodes.
*/
class composite_node_t : public node_t
{
public:

    /// Constructor.
    composite_node_t();

    /// Destructor.
	virtual ~composite_node_t();

    /// Called for the new node, after being copied.
    virtual void cloned();

	/// Dispatch function for visitor pattern.
	virtual void accept( node_visitor& v);

    /// Returns a const reference to the node graph inside this node.
    const node_graph_t& graph() const { return g_;}

    /// Returns a reference to the node graph inside this node.
    node_graph_t& graph() { return g_;}

	typedef node_graph_t::const_node_iterator const_iterator;

	/// Returns a const iterator to the first children node.
	const_iterator begin() const { return g_.nodes_begin();}

	/// Returns a const end iterator.
	const_iterator end() const { return g_.nodes_end();}

	typedef node_graph_t::node_iterator iterator;

    /// Returns an iterator to the first children node.
	iterator begin() { return g_.nodes_begin();}

	/// Returns an end iterator.
	iterator end() { return g_.nodes_end();}

    /// Creates a new node as a child of this node.
    node_t *create_node_by_id( const std::string& id, bool ui = false);

    /// Creates a new node as a child of this node.
    node_t *create_node_by_id_with_version( const std::string& id, const std::pair<int, int>& version);

    /// Adds a node as a child of this node.
    virtual void add_node( std::auto_ptr<node_t> n);

    /// Removes a child node from this node.
    virtual std::auto_ptr<node_t> remove_node( node_t *n);

    /// Finds the children node with the given name.
    const node_t *find_node( const std::string& name) const;

    /// Finds the children node with the given name.
    node_t *find_node( const std::string& name);

    void all_children_node_names( std::set<std::string>& names) const;

    /// Returns a const reference to this node graph layout.
    const ui::graph_layout_t& layout() const { return *layout_;}

    /// Returns a reference to this node graph layout.
    ui::graph_layout_t& layout() { return *layout_;}

    /// Sets this node graph layout.
    void set_layout( std::auto_ptr<ui::graph_layout_t> layout);

protected:

	composite_node_t( const composite_node_t& other);
	void operator=( const composite_node_t&);

    // node factory
    std::auto_ptr<node_t> do_create_node_by_id( const std::string& id, bool ui = false) const;
    std::auto_ptr<node_t> do_create_node_by_id_with_version( const std::string& id, const std::pair<int, int>& version) const;

private:

    /*!
        \brief Customization hook for node_t::read.
        Implement in subclasses to read extra data from node.
    */
    virtual void do_read( const serialization::yaml_node_t& in, const std::pair<int,int>& version);

    /*!
        \brief Customization hook for node_t::write.
        Implement in subclasses to write extra data to out.
    */
    virtual void do_write( serialization::yaml_oarchive_t& out) const;

    std::auto_ptr<node_t> create_unknown_node( const std::string& id, const std::pair<int, int>& version) const;

    void read_node( const serialization::yaml_node_t& in);

    void read_edge( const serialization::yaml_node_t& in);
    void write_edge( serialization::yaml_oarchive_t& out, const edge_t& e) const;

	node_graph_t g_;
	std::auto_ptr<ui::graph_layout_t> layout_;
};

} // ramen

#endif
