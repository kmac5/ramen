// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_COMPOSITION_HPP
#define	RAMEN_COMPOSITION_HPP

#include<ramen/config.hpp>

#include<vector>
#include<utility>
#include<memory>

#include<boost/noncopyable.hpp>

#include<ramen/nodes/node_graph.hpp>
#include<ramen/nodes/node.hpp>

#include<ramen/container/unique_name_map.hpp>

#include<ramen/render/context.hpp>

#include<ramen/image/format.hpp>

namespace ramen
{

/*!
\ingroup app
\brief Composition class.
*/
class composition_t : boost::noncopyable
{
public:

    typedef node_graph_t graph_type;

    typedef graph_type::node_iterator		node_iterator;
    typedef graph_type::const_node_iterator	const_node_iterator;

    typedef graph_type::reverse_node_iterator		reverse_node_iterator;
    typedef graph_type::const_reverse_node_iterator	const_reverse_node_iterator;

    typedef graph_type::node_range_type         node_range_type;
    typedef graph_type::const_node_range_type	const_node_range_type;

    typedef graph_type::edge_iterator		edge_iterator;
    typedef graph_type::const_edge_iterator	const_edge_iterator;

    typedef graph_type::reverse_edge_iterator		reverse_edge_iterator;
    typedef graph_type::const_reverse_edge_iterator	const_reverse_edge_iterator;

    typedef graph_type::edge_range_type         edge_range_type;
    typedef graph_type::const_edge_range_type	const_edge_range_type;

    composition_t();
    ~composition_t();

    void add_node( std::auto_ptr<node_t> n);
    std::auto_ptr<node_t> release_node( node_t *n);
    node_t *find_node( const std::string& name);

    void add_edge( const edge_t& e, bool notify = false);
    void remove_edge( const edge_t& e, bool notify = false);

	void merge_composition( composition_t& other);

    node_range_type& nodes()                { return g_.nodes();}
    const_node_range_type& nodes() const	{ return g_.nodes();}

    const_edge_range_type& edges() const	{ return g_.edges();}
    edge_range_type& edges()                { return g_.edges();}

	// import composition needs access to this...
    const unique_name_map_t<node_t*>& node_map() const;
	std::string make_name_unique( const std::string& s) const;
	
    // observer
    typedef boost::signals2::connection connection_type;
    typedef boost::signals2::signal<void ( node_t*)> signal_type;

    // called when a node is added, used in the inspector and viewer
    connection_type attach_add_observer( signal_type::slot_function_type subscriber)
    {
        return added_.connect( subscriber);
    }

    // called when a node is released, used in the inspector and viewer
    connection_type attach_release_observer( signal_type::slot_function_type subscriber)
    {
        return released_.connect( subscriber);
    }

    // connections
    bool can_connect( node_t *src, node_t *dst, int port);
    void connect( node_t *src, node_t *dst, int port);
    void disconnect( node_t *src, node_t *dst, int port);

	// misc
    void rename_node( node_t *n, const std::string& new_name);

	// notifications
	void notify_all_dirty();
	void clear_all_notify_dirty_flags();
	
	// interacting
	void begin_interaction();
	void end_interaction();

	// settings
    int start_frame() const	{ return start_frame_;}
    int end_frame() const	{ return end_frame_;}
    
    void set_start_frame( int f)    { start_frame_ = f;}
    void set_end_frame( int f)      { end_frame_ = f;}

    float frame() const		{ return frame_;}
    void set_frame( float f);

    bool autokey() const        { return autokey_;}
    void set_autokey( bool b)   { autokey_ = b;}

    render::context_t current_context( render::render_mode mode = render::interface_render) const;

    image::format_t default_format() const             { return default_format_;}
    void set_default_format( const image::format_t& f) { default_format_ = f;}

    int frame_rate() const        { return frame_rate_;}
    void set_frame_rate( int f)   { frame_rate_ = f;}

	// color management
	int num_ocio_context_pairs() const;
	const std::vector<std::pair<std::string, std::string> >& ocio_context_pairs() const;
	bool set_ocio_context_key_value( int index, const std::string& key, const std::string& value);

	boost::signals2::signal<void ()> ocio_context_changed;
	
    // selections
    typedef boost::signals2::signal<void ()> selection_signal_type;
    selection_signal_type selection_changed;

    connection_type attach_selection_observer( selection_signal_type::slot_function_type subscriber)
    {
        return selection_changed.connect( subscriber);
    }

	void select_all();
    void deselect_all();

    bool any_selected() const;
    node_t *selected_node();

	// composition settings
	boost::signals2::signal<void ( composition_t *)> settings_changed;
	
    // home directory, for relative paths
    const boost::filesystem::path& composition_dir() const;
    void set_composition_dir( const boost::filesystem::path& dir);
	
	// convert
	void convert_all_relative_paths( const boost::filesystem::path& new_base);
	void make_all_paths_absolute();	
	void make_all_paths_relative();
	
    boost::filesystem::path relative_to_absolute( const boost::filesystem::path& p) const;
    boost::filesystem::path absolute_to_relative( const boost::filesystem::path& p) const;

    // serialization
	void load_from_file( const boost::filesystem::path& p);
	
private:
		
	std::auto_ptr<node_t> create_node( const std::string& id, const std::pair<int,int>& version);
	std::auto_ptr<node_t> create_unknown_node( const std::string& id, const std::pair<int, int>& version);

    boost::filesystem::path composition_dir_;

    graph_type g_;
    unique_name_map_t<node_t*> node_map_;

    signal_type added_;
    signal_type released_;

    int start_frame_, end_frame_;
    float frame_;
    image::format_t default_format_;
    int frame_rate_;
    bool autokey_;
	
	std::vector<std::pair<std::string, std::string> > ocio_context_pairs_;
};

} // ramen

#endif
