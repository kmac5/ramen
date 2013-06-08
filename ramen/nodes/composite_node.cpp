// Copyright (c) 2012 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/composite_node.hpp>

#include<boost/bind.hpp>
#include<boost/foreach.hpp>

#include<boost/range/algorithm/for_each.hpp>

#include<ramen/assert.hpp>

#include<ramen/nodes/world_node.hpp>
#include<ramen/nodes/node_factory.hpp>

#include<ramen/ui/graph_layout.hpp>

namespace ramen
{

composite_node_t::composite_node_t() : node_t()
{
    layout_.reset( new ui::simple_graph_layout_t());
}

composite_node_t::composite_node_t( const composite_node_t& other) : node_t( other), g_( other.g_)
{
    layout_.reset( other.layout_->clone());
}

composite_node_t::~composite_node_t() {}

void composite_node_t::cloned()
{
    boost::range::for_each( graph().nodes(), boost::bind( &node_t::cloned, _1));
}

void composite_node_t::accept( node_visitor& v) { v.visit( this);}

node_t *composite_node_t::create_node_by_id( const std::string& id, bool ui)
{
    std::auto_ptr<node_t> n( do_create_node_by_id( id, ui));
    node_t *nn = n.get();

    if( nn)
        add_node( n);

    return nn;
}

std::auto_ptr<node_t> composite_node_t::do_create_node_by_id( const std::string& id, bool ui) const
{
    return node_factory_t::instance().create_by_id( id, ui);
}

node_t *composite_node_t::create_node_by_id_with_version( const std::string& id, const std::pair<int, int>& version)
{
    std::auto_ptr<node_t> n( do_create_node_by_id_with_version( id, version));
    node_t *nn = n.get();

    if( nn)
        add_node( n);

    return nn;
}

std::auto_ptr<node_t> composite_node_t::do_create_node_by_id_with_version( const std::string& id, const std::pair<int, int>& version) const
{
    return node_factory_t::instance().create_by_id_with_version( id, version);
}

std::auto_ptr<node_t> composite_node_t::create_unknown_node( const std::string& id, const std::pair<int, int>& version) const
{
    // TODO: implent this.
    return std::auto_ptr<node_t>();
    RAMEN_ASSERT( false);
}

void composite_node_t::add_node( std::auto_ptr<node_t> n)
{
    RAMEN_ASSERT( false);

    /*
    RAMEN_ASSERT( n.get());

    node_t *nn = n.get();
    g_.add_node( n);
    nn->set_parent( this);
    // rename node here, if needed, more stuff, ...

    if( world_node_t *w = world())
        w->node_added( nn);
    */
}

std::auto_ptr<node_t> composite_node_t::remove_node( node_t *n)
{
    // TODO: implement this.
    RAMEN_ASSERT( false);

    /*
    n->set_parent( 0);

    if( world_node_t *w = world())
        w->node_removed( n);

    return g_.remove_node( n);
    */
}

const node_t *composite_node_t::find_node( const std::string& name) const
{
    composite_node_t& self = const_cast<composite_node_t&>( *this);
    return self.find_node( name);
}

node_t *composite_node_t::find_node( const std::string& name)
{
    BOOST_FOREACH( node_t& n, graph().nodes())
    {
        if( n.name() == name)
            return &n;

        if( composite_node_t *cn = dynamic_cast<composite_node_t*>( &n))
        {
            node_t *nn = cn->find_node( name);

            if( nn)
                return nn;
        }
    }

    return 0;
}

void composite_node_t::all_children_node_names( std::set<std::string>& names) const
{
    BOOST_FOREACH( const node_t& n, graph().nodes())
    {
        names.insert( n.name());

        if( const composite_node_t *cn = dynamic_cast<const composite_node_t*>( &n))
            cn->all_children_node_names( names);
    }
}

void composite_node_t::set_layout( std::auto_ptr<ui::graph_layout_t> layout) { layout_ = layout;}

void composite_node_t::do_read(const serialization::yaml_node_t& in, const std::pair<int,int>& version)
{
    serialization::yaml_node_t nodes = in.get_node( "children").get_node( "nodes");

    for( int i = 0; i < nodes.size(); ++i)
        read_node( nodes[i]);

    serialization::yaml_node_t edges = in.get_node( "children").get_node( "edges");

    for( int i = 0; i < edges.size(); ++i)
        read_edge( edges[i]);
}

void composite_node_t::do_write( serialization::yaml_oarchive_t& out) const
{
    out << YAML::Key << "children" << YAML::Value;
        out.begin_map();
            out << YAML::Key << "nodes" << YAML::Value;
            out.begin_seq();
                boost::range::for_each( graph().nodes(),
                                        boost::bind( &node_t::write, _1, boost::ref( out)));
            out.end_seq();

        out << YAML::Key << "edges" << YAML::Value;
            out.begin_seq();
                boost::range::for_each( graph().edges(),
                                        boost::bind( &composite_node_t::write_edge, this, boost::ref( out), _1));
            out.end_seq();
        out.end_map();
}

void composite_node_t::read_node(const serialization::yaml_node_t& in)
{
    // TODO: implent this.
    serialization::yaml_node_t class_node( in.get_node( "class"));

    std::string id;
    class_node[0] >> id;

    std::pair<int,int> version;
    class_node[1] >> version.first;
    class_node[2] >> version.second;

    std::auto_ptr<node_t> p( create_node_by_id_with_version( id, version));

    if( !p.get())
    {
        in.error_stream() << "Error creating node: " << id << "\n";
        return;
    }

    //p->set_composition( this); // some nodes needs this set early...
    p->read( in, version);
    //p->set_frame( frame_);

    /*
    if( image_node_t *img_node = dynamic_cast<image_node_t*>( p.get()))
    {
        render::context_t context = current_context();
        img_node->calc_format( context);
        img_node->format_changed();
    }
    */

    //node_map_.insert( p.get());
    //g_.add_node( p);

    RAMEN_ASSERT( false);
}

void composite_node_t::read_edge(const serialization::yaml_node_t& in)
{
    // TODO: implent this.
    RAMEN_ASSERT( false);
}

void composite_node_t::write_edge( serialization::yaml_oarchive_t& out, const edge_t& e) const
{
    out.flow();
    out.begin_seq();
        out << e.src->name() << e.dst->name() << e.port;
    out.end_seq();
}

} // namespace
