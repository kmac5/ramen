// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/node.hpp>

#include<vector>
#include<stdexcept>
#include<algorithm>

#include<boost/bind.hpp>
#include<boost/foreach.hpp>
#include<boost/range/algorithm/for_each.hpp>

#include<ramen/nodes/graph_algorithm.hpp>

#include<ramen/app/composition.hpp>

#include<ramen/serialization/yaml_iarchive.hpp>
#include<ramen/serialization/yaml_oarchive.hpp>

#include<ramen/util/string.hpp>
#include<ramen/util/flags.hpp>

namespace ramen
{
namespace
{

struct frames_needed_less
{
    bool operator()( const std::pair<int, int>& a, const std::pair<int, int>& b) const
    {
        if( a.first < b.first)
            return true;

        if( a.first == b.first)
            return a.second < b.second;

        return false;
    }
};

} // unnamed

node_t::node_t() : composite_parameterised_t(), flags_( 0), composition_( 0) {}

node_t::node_t( const node_t& other) : composite_parameterised_t( other), outputs_( other.outputs_)
{
    boost::range::for_each( outputs_, boost::bind( &node_output_plug_t::set_parent_node, _1, this));
    flags_ = other.flags_;
    loc_ = other.loc_;
    composition_ = other.composition_;
}

node_t::~node_t() {}

void node_t::cloned()
{
    for( int i = 0; i < num_inputs(); ++i)
        connected( 0, i);

    create_manipulators();
}

// visitor
void node_t::accept( node_visitor& v) { v.visit( this);}

bool node_t::selected() const { return flags_ & selected_bit;}

void node_t::select( bool state)
{
    if( state)
    {
        if( ui_invisible())
        {
            // ui invisible nodes can't be selected
            flags_ &= ~selected_bit;
        }
        else
            flags_ |= selected_bit;
    }
    else
        flags_ &= ~selected_bit;
}

void node_t::toggle_selection()	{ select( !selected());}

bool node_t::ignored() const        { return util::test_flag( flags_, ignored_bit);}
void node_t::set_ignored( bool b)   { util::set_flag( flags_, ignored_bit, b );}

bool node_t::plugin_error() const       { return util::test_flag( flags_, plugin_error_bit);}
void node_t::set_plugin_error( bool b)  { util::set_flag( flags_, plugin_error_bit, b );}

bool node_t::autolayout() const         { return util::test_flag( flags_, autolayout_bit);}
void node_t::set_autolayout( bool b)    { util::set_flag( flags_, autolayout_bit, b );}

bool node_t::cacheable() const          { return flags_ & cacheable_bit;}
void node_t::set_cacheable( bool b)     { util::set_flag( flags_, cacheable_bit, b );}

bool node_t::notify_dirty() const       { return util::test_flag( flags_, notify_dirty_bit);}
void node_t::set_notify_dirty( bool b)  { util::set_flag( flags_, notify_dirty_bit, b );}

bool node_t::ui_invisible() const       { return flags_ & ui_invisible_bit;}
void node_t::set_ui_invisible( bool b)  { util::set_flag( flags_, ui_invisible_bit, b );}

bool node_t::is_active() const     { return util::test_flag( flags_, active_bit);}
bool node_t::is_context() const    { return util::test_flag( flags_, context_bit);}

// inputs

int node_t::find_input( const adobe::name_t& id) const
{
    int index = 0;
    BOOST_FOREACH( const node_input_plug_t& i, input_plugs())
    {
        if( i.id().c_str() == id.c_str() )
            return index;

        ++index;
    }

    return -1;
}

const node_t *node_t::input( std::size_t i) const
{
    RAMEN_ASSERT( i < inputs_.size());
    return inputs_[i].input_node();
}

node_t *node_t::input( std::size_t i)
{
    RAMEN_ASSERT( i < inputs_.size());
    return inputs_[i].input_node();
}

void node_t::add_input_plug( const std::string &id, bool optional, const Imath::Color3c &color, const std::string &tooltip)
{
    inputs_.push_back( node_input_plug_t( id, optional, color, tooltip ));
}

std::size_t node_t::num_outputs() const
{
    if( !has_output_plug())
        return 0;

    return outputs_[0].connections().size();
}

const node_output_plug_t& node_t::output_plug() const
{
    RAMEN_ASSERT( has_output_plug());
    return outputs_[0];
}

node_output_plug_t& node_t::output_plug()
{
    RAMEN_ASSERT( has_output_plug());
    return outputs_[0];
}

const node_t *node_t::output( std::size_t i) const
{
    RAMEN_ASSERT( has_output_plug());
    RAMEN_ASSERT( i < num_outputs());
    return boost::get<0>( outputs_[0].connections()[i] );
}

node_t *node_t::output( std::size_t i)
{
    RAMEN_ASSERT( has_output_plug());
    RAMEN_ASSERT( i < num_outputs());
    return boost::get<0>( outputs_[0].connections()[i] );
}

void node_t::add_output_plug( const std::string &id, const Imath::Color3c& color, const std::string& tooltip )
{
    RAMEN_ASSERT( !has_output_plug());
    outputs_.push_back( new node_output_plug_t( this, id, color, tooltip ));
}

bool node_t::accept_connection( node_t *src, int port) const { return true;}

void node_t::connected( node_t *src, int port)
{
    if( variable_num_inputs() && src != 0 && port == num_inputs()-1)
        add_new_input_plug();

    do_connected( src, port);
}

void node_t::do_connected( node_t *src, int port) {}

void node_t::add_new_input_plug()
{
    RAMEN_ASSERT( 0 );
    /*
    add_input_plug( input_plug_info_t( ui::palette_t::instance().color("back plug")), true);
    reconnect_node();
    */
}

void node_t::reconnect_node()
{
    composition_t *comp = composition();

    if( comp)
    {
        BOOST_FOREACH( edge_t& e, comp->edges())
        {
            if( e.dst == this)
                input_plugs()[ e.port].set_input( e.src, adobe::name_t( "unused"));
        }
    }
}

// params
void node_t::param_edit_finished() { notify();}

void node_t::notify()
{
    breadth_first_outputs_search( *this, boost::bind( &node_t::do_notify, _1));
}

void node_t::do_notify()
{
    RAMEN_ASSERT( composition());

    changed( this);
    set_notify_dirty( false);
}

void node_t::calc_frames_needed( const render::context_t& context)
{
    do_calc_frames_needed( context);

    if( !frames_needed().empty())
        std::sort( frames_needed().begin(), frames_needed().end(), frames_needed_less());
}

void node_t::do_calc_frames_needed( const render::context_t& context) {}

void node_t::begin_active()
{
    flags_ |= active_bit;
    do_begin_active();
}

void node_t::end_active()
{
    do_end_active();
    flags_ &= ~active_bit;
}

void node_t::begin_context()
{
    flags_ |= context_bit;
    do_begin_context();
}

void node_t::end_context()
{
    do_end_context();
    flags_ &= ~context_bit;
}

bool node_t::interacting() const
{
    return flags_ & interacting_bit;
}

void node_t::begin_interaction()
{
    do_begin_interaction();
    flags_ |= interacting_bit;
}

void node_t::end_interaction()
{
    do_end_interaction();
    flags_ &= ~interacting_bit;
}

bool node_t::is_valid() const
{
    BOOST_FOREACH( const node_input_plug_t& i, input_plugs())
    {
        if( !i.connected() && !i.optional())
            return false;
    }

    if( ignored())
    {
        if( num_inputs() == 0)
            return false;

        // handle the all inputs optional case
        bool all_optional = true;
        bool all_disconnected = true;

        BOOST_FOREACH( const node_input_plug_t& i, input_plugs())
        {
            if( i.connected())
                all_disconnected = false;

            if( !i.optional())
                all_optional = false;
        }

        if( all_optional && all_disconnected)
            return false;
    }

    return do_is_valid();
}

bool node_t::do_is_valid() const { return true;}

bool node_t::is_identity() const
{
    RAMEN_ASSERT( is_valid());

    if( ignored())
        return true;

    // generators can never be identity
    if( num_inputs() == 0)
        return false;

    return do_is_identity();
}

bool node_t::do_is_identity() const { return false;}

// hash
void node_t::clear_hash()
{
    hash_generator().reset();

    set_cacheable( !is_frame_varying());

    if( cacheable())
    {
        BOOST_FOREACH( node_input_plug_t& i, input_plugs())
        {
            if( i.connected())
            {
                if( !i.input_node()->cacheable())
                {
                    set_cacheable( false);
                    return;
                }
            }
        }
    }

    frames_needed().clear();
}

void node_t::calc_hash_str( const render::context_t& context)
{
    RAMEN_ASSERT( hash_generator().empty());

    if( !cacheable())
        return;

    bool valid = is_valid();

    if( valid && is_identity())
    {
        if( num_inputs() > 0 && input())
            hash_generator() << input()->hash_str();

        return;
    }

    hash_generator() << typeid( *this).name();

    if( !valid)
    {
        add_context_to_hash_string( context);
        return;
    }

    // handle the case when all inputs are optional and disconnected.
    if( num_inputs() != 0)
    {
        bool all_optional = true;
        bool not_connected = true;

        for( int i = 0; i < num_inputs(); ++i)
        {
            if( !input_plugs()[i].optional())
                all_optional = false;

            if( input(i))
                not_connected = false;
        }

        if( all_optional && not_connected)
            add_context_to_hash_string( context);
    }

    // for each needed input frame...
    calc_frames_needed( context);

    if( !frames_needed_.empty() && !ignored())
        add_needed_frames_to_hash( context);
    else
    {
        for( int i = 0; i < num_inputs(); ++i)
            if( input( i) && include_input_in_hash( i))
                hash_generator() << i << input( i)->hash_str();
    }

    do_calc_hash_str( context);
}

void node_t::do_calc_hash_str( const render::context_t& context)
{
    param_set().add_to_hash( hash_generator());
}

std::string node_t::hash_str() const { return hash_generator().str();}

const hash::generator_t::digest_type& node_t::digest() { return hash_generator().digest();}

void node_t::add_context_to_hash_string( const render::context_t& context)
{
    hash_generator() << context.subsample;
    hash_generator() << context.motion_blur_extra_samples;
    hash_generator() << context.motion_blur_shutter_factor;
}

bool node_t::include_input_in_hash( int num) const { return true;}

void node_t::add_needed_frames_to_hash( const render::context_t& context)
{
    const_frames_needed_iterator it( frames_needed().begin()), end( frames_needed().end());

    while( it != end)
    {
        float cur_frame = it->first;
        render::context_t new_context( context);
        new_context.frame = cur_frame;

        while( 1)
        {
            node_t *n = input( it->second);
            depth_first_inputs_search( *n, boost::bind( &node_t::set_frame, _1, cur_frame));
            depth_first_inputs_search( *n, boost::bind( &node_t::clear_hash, _1));
            depth_first_inputs_search( *n, boost::bind( &node_t::calc_hash_str, _1, new_context));
            hash_generator() << it->second << n->hash_str();
            ++it;

            if( it == end || it->first != cur_frame)
                break;
        }
    }

    // restore original time here
    for( int i = 0; i < num_inputs(); ++i)
    {
        if( node_t *n = input( i))
        {
            depth_first_inputs_search( *n, boost::bind( &node_t::set_frame, _1, context.frame));
            depth_first_inputs_search( *n, boost::bind( &node_t::clear_hash, _1));
            depth_first_inputs_search( *n, boost::bind( &node_t::calc_hash_str, _1, context));
        }
    }
}

// cache
bool node_t::is_frame_varying() const { return false;}

// ui
const char *node_t::help_string() const
{
    RAMEN_ASSERT( metaclass());
    return metaclass()->help;
}

void node_t::convert_relative_paths( const boost::filesystem::path& old_base, const boost::filesystem::path& new_base)
{
    boost::range::for_each( param_set(),
                            boost::bind( &param_t::convert_relative_paths, _1, old_base, new_base));
}

void node_t::make_paths_absolute()
{
    boost::range::for_each( param_set(),
                            boost::bind( &param_t::make_paths_absolute, _1));
}

void node_t::make_paths_relative()
{
    boost::range::for_each( param_set(), boost::bind( &param_t::make_paths_relative, _1));
}

// serialization
void node_t::read(const serialization::yaml_node_t& in, const std::pair<int,int>& version)
{
    std::string n;
    in.get_value( "name", n);

    RAMEN_ASSERT( util::is_string_valid_identifier( n));

    if( !util::is_string_valid_identifier( n))
        throw std::runtime_error( "Bad name in node_t");

    set_name( n);

    if( !in.get_optional_value( "comp_pos", loc_))
        set_autolayout( true);

    // create needed extra inputs if needed.
    if( variable_num_inputs())
    {
        int num_ins = num_inputs();
        in.get_optional_value( "num_inputs", num_ins);

        while( num_ins != num_inputs())
            add_new_input_plug();
    }

    bool flag = false;
    if( in.get_optional_value( "ignored", flag))
        set_ignored( flag);

    serialization::yaml_node_t prms( in.get_node( "params"));
    param_set().read( prms);

    do_read( in, version);
}

void node_t::do_read( const serialization::yaml_node_t& in, const std::pair<int,int>& version) {}

void node_t::write( serialization::yaml_oarchive_t& out) const
{
    RAMEN_ASSERT( metaclass() && "Trying to serialize an abstract node");
    out.begin_map();
        write_node_info( out);
        param_set().write( out);
        do_write( out);
    out.end_map();
}

void node_t::do_write( serialization::yaml_oarchive_t& out) const {}

void node_t::write_node_info( serialization::yaml_oarchive_t& out) const
{
    out << YAML::Key << "class" << YAML::Value;
    out.flow();
        out.begin_seq();
        out << metaclass()->id
            << metaclass()->major_version << metaclass()->minor_version;
        out.end_seq();

    out << YAML::Key << "name"  << YAML::DoubleQuoted << YAML::Value << name();
    out << YAML::Key << "comp_pos" << YAML::Value << location();

    if( ignored())
        out << YAML::Key << "ignored" << YAML::Value << true;

    if( variable_num_inputs())
        out << YAML::Key << "num_inputs" << YAML::Value << num_inputs();
}

node_t *new_clone( const node_t& other)
{
    return dynamic_cast<node_t*>( new_clone( dynamic_cast<const parameterised_t&>( other)));
}

} // namespace
