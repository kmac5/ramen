// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_NODES_PLUG_HPP
#define RAMEN_NODES_PLUG_HPP

#include<utility>
#include<vector>
#include<algorithm>

#include<boost/noncopyable.hpp>
#include<boost/tuple/tuple.hpp>

#include<adobe/name.hpp>

#include<OpenEXR/ImathColor.h>

#include<ramen/assert.hpp>

#include<ramen/dependency/node.hpp>

#include<ramen/nodes/node_fwd.hpp>

namespace ramen
{

/*!
\ingroup nodes
\brief Base class for node plugs.
*/
class node_plug_t
{
public:

    /// Constructor.
	node_plug_t( const std::string& id, const Imath::Color3c& color, const std::string& tooltip)
	{
		id_ = adobe::name_t( id.c_str());
		color_ = color;
		tooltip_ = adobe::name_t( tooltip.c_str());
	}

    /// Copy constructor.
	node_plug_t( const node_plug_t& other) : tooltip_( other.tooltip_), color_( other.color_), id_( other.id_) {}

    /// Returns this plug id.
	const adobe::name_t& id() const { return id_;}

    /// Returns this plug color. Used in the UI.
	const Imath::Color3c& color() const	{ return color_;}

    /// Returns this plug tooltip. Used in the UI.
    const adobe::name_t& tooltip() const	{ return tooltip_;}

    /// Operator less, for assoc. containers (future).
    bool operator<( const node_plug_t& other) const
    {
        // compare pointers directly.
        return id().c_str() < other.id().c_str();
    }

private:

	adobe::name_t id_;
	Imath::Color3c color_;
	adobe::name_t tooltip_;
};

/*!
\ingroup nodes
\brief An input plug to which other node plugs can be connected.
*/
class node_input_plug_t : public node_plug_t
{
public:

    typedef std::pair<ramen::node_t*,adobe::name_t> connection_t;

    node_input_plug_t( const std::string& id, bool optional,
					   const Imath::Color3c& color, const std::string& tooltip) : node_plug_t( id, color, tooltip)
	{
		input_.first = 0;
		optional_ = optional;
	}

    /// Copy constructor.
	node_input_plug_t( const node_input_plug_t& other) : node_plug_t( other)
	{
        input_.first = 0;
		optional_ = other.optional();
	}

    /// Returns if this plug is optional.
	bool optional() const { return optional_;}

    /// Returns true if there's a node connected to this plug.
	bool connected() const	{ return input_.first != 0;}

    /// Returns the node connected to this plug, or null.
    const ramen::node_t *input_node() const { return input_.first;}

    /// Returns the node connected to this plug, or null.
    ramen::node_t *input_node() { return input_.first;}

    /// Returns the output plug id of the input node this plug is connected to.
    const adobe::name_t& input_node_out_plug() const { return input_.second;}

    /// Sets the node and plug this plug is connected to.
	void set_input( ramen::node_t *n)
    {
        input_.first = n;
        input_.second = adobe::name_t( "unused");
    }

    /// Sets the node and plug this plug is connected to.
	void set_input( ramen::node_t *n, const adobe::name_t& plug)
    {
        input_.first = n;
        input_.second = plug;
    }

    /// Clear this plug connection.
    void clear_input() { input_.first = 0;}

private:

    connection_t input_;
    bool optional_;
};

/*!
\ingroup nodes
\brief An output plug to which other node plugs can be connected.
*/
class node_output_plug_t : public node_plug_t, public dependency::output_node_t
{
public:

    typedef boost::tuples::tuple<ramen::node_t*,adobe::name_t, int> connection_t;

    /// Constructor.
	node_output_plug_t( ramen::node_t *parent, const std::string& id,
						const Imath::Color3c& color, const std::string& tooltip);

    virtual ~node_output_plug_t();

    node_output_plug_t *clone() const { return do_clone();}

    const ramen::node_t *parent_node() const { return parent_;}
    ramen::node_t *parent_node()             { return parent_;}

    void set_parent_node( ramen::node_t *parent)
    {
        RAMEN_ASSERT( parent);

        parent_ = parent;
    }

    /// Adds a connection to this plug.
	void add_output( ramen::node_t *n, const adobe::name_t& plug);

    /// Adds a connection to this plug.
	void add_output( ramen::node_t *n, int port);

    /// Removes a connection to this plug.
    void remove_output( ramen::node_t *n, const adobe::name_t& plug);

    /// Removes a connection to this plug.
    void remove_output( ramen::node_t *n, int port);

    typedef std::vector<connection_t >::const_iterator  const_iterator;
    typedef std::vector<connection_t >::iterator        iterator;

    /// Returns a vector of connections from this plug.
	const std::vector<connection_t >& connections() const { return connections_;}

    /// Returns a vector of connections from this plug.
    std::vector<connection_t >& connections() { return connections_;}

protected:

    node_output_plug_t( const node_output_plug_t& other);

private:

    node_output_plug_t *do_clone() const;

	ramen::node_t *parent_;
	std::vector<connection_t > connections_;
};

RAMEN_API node_output_plug_t *new_clone( const node_output_plug_t& other);

} // namespace

#endif

