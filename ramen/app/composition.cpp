// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/version.hpp>

#include<ramen/app/composition.hpp>

#include<boost/foreach.hpp>
#include<boost/bind.hpp>
#include<boost/filesystem/operations.hpp>
#include<boost/range/algorithm/for_each.hpp>
#include<boost/range/algorithm/count_if.hpp>
#include<boost/range/algorithm/find_if.hpp>

#include<ramen/app/application.hpp>

#include<ramen/nodes/graph_algorithm.hpp>

#include<ramen/nodes/node_factory.hpp>
#include<ramen/nodes/image_node.hpp>

#include<ramen/params/composite_param.hpp>

#include<ramen/serialization/yaml_iarchive.hpp>
#include<ramen/serialization/yaml_oarchive.hpp>

#include<iostream>

namespace ramen
{

composition_t::composition_t()
{
    start_frame_ = 1;
    end_frame_ = 100;
    frame_ = 1;
    default_format_ = app().preferences().default_format();
    frame_rate_ = app().preferences().frame_rate();
    autokey_ = true;
}

composition_t::~composition_t()
{
    for( node_iterator it( nodes().begin()); it != nodes().end(); ++it)
        released_( &(*it));
}

const unique_name_map_t<node_t*>& composition_t::node_map() const { return node_map_;}

std::string composition_t::make_name_unique( const std::string& s) const
{
	return node_map().make_name_unique( s);
}

void composition_t::add_node( std::auto_ptr<node_t> n)
{
    node_map_.insert( n.get());
    n->set_composition( this);
    n->set_frame( frame_);

    render::context_t context = current_context();

    if( image_node_t *nn = dynamic_cast<image_node_t*>( n.get()))
    {
        nn->calc_format( context_);
        nn->format_changed();
    }

    g_.add_node( n);
    added_( n.get());
}

std::auto_ptr<node_t> composition_t::release_node( node_t *n)
{
    released_( n);
    node_map_.remove( n->name());
    n->set_composition( 0);
    return g_.release_node( n);
}

node_t *composition_t::find_node( const std::string& name)
{ 
	return node_map_.find( name);
}

void composition_t::add_edge( const edge_t& e, bool notify)
{
    g_.add_edge( e);

    if( notify)
        e.dst->connected( e.src, e.port);
}

void composition_t::remove_edge( const edge_t& e, bool notify)
{
    g_.remove_edge( e);

    if( notify)
        e.dst->connected( 0, e.port);
}

void composition_t::merge_composition( composition_t& other)
{
	// convert all relative paths
	boost::filesystem::path doc_dir( composition_dir());

	if( doc_dir.empty())
		other.make_all_paths_absolute();
	else
		other.set_composition_dir( doc_dir);

	// rename all nodes from other composition
	unique_name_map_t<node_t*> nmap( node_map());

	BOOST_FOREACH( node_t& n, other.nodes())
	{
		std::string new_name( nmap.make_name_unique( n.name()));

		if( new_name != n.name())
			other.rename_node( &n, new_name);

		nmap.insert_null( n.name());
	}

    // save edges
    std::vector<edge_t> other_edges( other.edges());

    while( !other.edges().empty())
		other.remove_edge( other.edges()[0]);

    // transfer nodes
    while( !other.nodes().empty())
    {
		node_t& n = other.nodes()[0];
		std::auto_ptr<node_t> nn( other.release_node( &n));
		add_node( nn);
    }

    BOOST_FOREACH( edge_t& e, other_edges)
		add_edge( e);
}

void composition_t::rename_node( node_t *n, const std::string& new_name)
{
    node_map_.remove( n->name());
    n->set_name( new_name);
    node_map_.insert( n);
}

void composition_t::notify_all_dirty()
{
	for( node_iterator it( nodes().begin()), ie = nodes().end(); it != ie; ++it)
	{
		if( it->notify_dirty())
			detail::set_outputs_color( *it, black);
	}

	for( node_iterator it( nodes().begin()), ie = nodes().end(); it != ie; ++it)
	{
		if( it->notify_dirty())
			detail::breadth_first_outputs_recursive_search( *it, boost::bind( &node_t::do_notify, _1));
	}
}

void composition_t::clear_all_notify_dirty_flags()
{
    boost::range::for_each( nodes(), boost::bind( &node_t::set_notify_dirty, _1, false));
}

void composition_t::begin_interaction()
{
    boost::range::for_each( nodes(), boost::bind( &node_t::begin_interaction, _1));
}

void composition_t::end_interaction()
{
	clear_all_notify_dirty_flags();
    boost::range::for_each( nodes(), boost::bind( &node_t::end_interaction, _1));
	notify_all_dirty();
}

bool composition_t::can_connect( node_t *src, node_t *dst, int port)
{
	if( !src->has_output_plug() || dst->num_inputs() == 0)
		return false;

	if( port >= dst->num_inputs())
		return false;
		
    if( node_depends_on_node( *src, *dst))
        return false;

    return dst->accept_connection( src, port);
}

void composition_t::connect( node_t *src, node_t *dst, int port)
{
    g_.connect( src, dst, port);
    dst->connected( src, port);
}

void composition_t::disconnect( node_t *src, node_t *dst, int port)
{
    g_.disconnect( src, dst, port);
    dst->connected( 0, port);
}

void composition_t::set_frame( float f)
{
    if( frame_ != f)
    {
        frame_ = f;
        boost::range::for_each( nodes(), boost::bind( &node_t::set_frame, _1, f));
    }
}

render::context_t composition_t::current_context( render::render_mode mode) const
{
    render::context_t c;
    c.mode = mode;
    c.composition = const_cast<composition_t*>( this);
    c.subsample = 1;
    c.frame = frame_;
    c.default_format = default_format_;
    return c;
}

// selections
void composition_t::select_all()
{
    boost::range::for_each( nodes(), boost::bind( &node_t::select, _1, true));
}

void composition_t::deselect_all()
{
    boost::range::for_each( nodes(), boost::bind( &node_t::select, _1, false));
}

bool composition_t::any_selected() const
{
    return boost::range::find_if( nodes(), boost::bind( &node_t::selected, _1)) != nodes().end();
}

node_t *composition_t::selected_node()
{
    if( boost::range::count_if( nodes(), boost::bind( &node_t::selected, _1)) == 1)
    {
        node_iterator it( nodes().begin());

        for( ; it != nodes().end(); ++it)
        {
            if( it->selected())
                return &(*it);
        }
    }

    return 0;
}

const boost::filesystem::path& composition_t::composition_dir() const { return composition_dir_;}

void composition_t::set_composition_dir( const boost::filesystem::path& dir)
{
    RAMEN_ASSERT( !dir.empty() && dir.is_absolute());

	if( dir == composition_dir())
		return;

	if( !composition_dir().empty())
		convert_all_relative_paths( dir);

	composition_dir_ = dir;
}

void composition_t::convert_all_relative_paths( const boost::filesystem::path& new_base)
{
    boost::range::for_each( nodes(), boost::bind( &node_t::convert_relative_paths, _1, composition_dir_, new_base));
}

void composition_t::make_all_paths_absolute()
{
    boost::range::for_each( nodes(), boost::bind( &node_t::make_paths_absolute, _1));
}

void composition_t::make_all_paths_relative()
{
    boost::range::for_each( nodes(), boost::bind( &node_t::make_paths_relative, _1));
}

boost::filesystem::path composition_t::relative_to_absolute( const boost::filesystem::path& p) const
{
    RAMEN_ASSERT( !composition_dir_.empty());
    return filesystem::make_absolute_path( p, composition_dir());
}

boost::filesystem::path composition_t::absolute_to_relative( const boost::filesystem::path& p) const
{
    RAMEN_ASSERT( !composition_dir_.empty());
    return filesystem::make_relative_path( p, composition_dir());
}

// serialization
void composition_t::load_from_file( const boost::filesystem::path& p)
{
	RAMEN_ASSERT( p.is_absolute());
	
    boost::filesystem::ifstream ifs( p, serialization::yaml_iarchive_t::file_open_mode());

    if( !ifs.is_open() || !ifs.good())
		throw std::runtime_error( std::string( "Couldn't open input file ") + filesystem::file_string( p));

	std::auto_ptr<serialization::yaml_iarchive_t> in( new serialization::yaml_iarchive_t( ifs));
		
	if( !in->read_composition_header())
		throw std::runtime_error( std::string( "Couldn't open input file ") + filesystem::file_string( p));
		
	set_composition_dir( p.parent_path());
	read( *in);	
}

void composition_t::read( serialization::yaml_iarchive_t& in)
{
	in.get_optional_value( "start_frame", start_frame_);
	in.get_optional_value( "end_frame", end_frame_);
	in.get_optional_value( "frame", frame_);
	in.get_optional_value( "autokey", autokey_);
	in.get_optional_value( "format", default_format_);
	
	read_nodes( in);
	read_edges( in);
	
	BOOST_FOREACH( node_t& n, nodes())
		added_( &n);

	BOOST_FOREACH( node_t& n, nodes())
		n.for_each_param( boost::bind( &param_t::emit_param_changed, _1, param_t::node_loaded));
}

void composition_t::read_nodes( serialization::yaml_iarchive_t& in)
{
	serialization::yaml_node_t node = in.get_node( "nodes");

	for( int i = 0; i < node.size(); ++i)
		read_node( node[i]);
}

void composition_t::read_node( const serialization::yaml_node_t& node)
{
	try
	{
		serialization::yaml_node_t class_node( node.get_node( "class"));
	
		std::string id;
		std::pair<int,int> version;
	
		class_node[0] >> id;
		class_node[1] >> version.first;
		class_node[2] >> version.second;
	
		std::auto_ptr<node_t> p( create_node( id, version));
		
		if( !p.get())
		{
			node.error_stream() << "Error creating node: " << id << "\n";
			return;
		}
		
		p->set_composition( this); // some nodes needs this set early...
		
		p->read( node, version);
	    p->set_frame( frame_);

	    if( image_node_t *img_node = dynamic_cast<image_node_t*>( p.get()))
	    {
			render::context_t context = current_context();
	        img_node->calc_format( context);
	        img_node->format_changed();
	    }
	
	    node_map_.insert( p.get());
	    g_.add_node( p);
	}
	catch( YAML::Exception& e)
	{	
		node.error_stream() << "Yaml exception: " << e.what() << " while reading node\n";
	}
	catch( std::runtime_error& e)
	{
		node.error_stream() << "Error reading node\n";
	}
}

std::auto_ptr<node_t> composition_t::create_node( const std::string& id, const std::pair<int,int>& version)
{
	RAMEN_ASSERT( !id.empty());
	RAMEN_ASSERT( version.first >= 0 && version.second >= 0);
	
	std::auto_ptr<node_t> p( node_factory_t::instance().create_by_id_with_version( id, version));

	// as a last resort, return an unknown node
    if( !p.get())
		return create_unknown_node( id, version);

	try
	{
		p->set_composition( this);
	    p->create_params();
	    p->create_manipulators();
	}
	catch( ...)
	{
		return create_unknown_node( id, version);
	}
	
	return p;
}

std::auto_ptr<node_t> composition_t::create_unknown_node( const std::string& id, const std::pair<int,int>& version)
{
	// TODO: implement this.
	RAMEN_ASSERT( 0 && "Create unknown node not implemented yet");
    return std::auto_ptr<node_t>();
}

void composition_t::read_edges( const serialization::yaml_iarchive_t& in)
{
	serialization::yaml_node_t node = in.get_node( "edges");

	for( int i = 0; i < node.size(); ++i)
		read_edge( node[i]);
}

void composition_t::read_edge( const serialization::yaml_node_t& node)
{
	std::string src, dst;
	int port;

	node[0] >> src;
	node[1] >> dst;
	node[2] >> port;

	node_t *src_ptr = find_node( src);
	node_t *dst_ptr = find_node( dst);
	
	if( !src_ptr)
	{
		node.error_stream() << "Error: Couldn't find node " << src << "\n";
		return;
	}

	if( !dst_ptr)
	{
		node.error_stream() << "Error: Couldn't find node " << dst << "\n";
		return;
	}
	
    // TODO: fix this. (est.)
	// if any of the nodes is unknown, add the needed plugs.
	//if( src_ptr->plugin_error() && !src_ptr->has_output_plug())
	//	src_ptr->add_output_plug();
	
	if( dst_ptr->plugin_error())
	{
		while( dst_ptr->num_inputs() < ( port + 1))
			dst_ptr->add_new_input_plug();
	}

	if( can_connect( src_ptr, dst_ptr, port))
	{
		connect( src_ptr, dst_ptr, port);
		dst_ptr->notify();
	}
	else
		node.error_stream() << "Can't connect " << src_ptr->name() << " to " << dst_ptr->name() << "\n";
}

void composition_t::write( serialization::yaml_oarchive_t& out) const
{
    out << YAML::Key << "start_frame"   << YAML::Value << start_frame_;
    out << YAML::Key << "end_frame"     << YAML::Value << end_frame_;
    out << YAML::Key << "frame"         << YAML::Value << frame_;
    out << YAML::Key << "autokey"       << YAML::Value << autokey_;
    out << YAML::Key << "format"        << YAML::Value << default_format_;
	out.check_errors();
	
    out << YAML::Key << "nodes" << YAML::Value;
		out.begin_seq();
            boost::range::for_each( nodes(), boost::bind( &node_t::write, _1, boost::ref( out)));
        out.end_seq();

	out << YAML::Key << "edges" << YAML::Value;
        out.begin_seq();
            boost::range::for_each( edges(), boost::bind( &composition_t::write_edge, this, boost::ref( out), _1));
		out.end_seq();
}

void composition_t::write_edge( serialization::yaml_oarchive_t& out, const edge_t& e) const
{
    out.flow();
	out.begin_seq();
        out << e.src->name() << e.dst->name() << e.port;
    out.end_seq();
}

} // ramen
