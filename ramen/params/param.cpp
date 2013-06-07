// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/params/param.hpp>

#include<stdexcept>

#include<ramen/params/parameterised.hpp>

#include<ramen/serialization/yaml_oarchive.hpp>

#include<ramen/util/string.hpp>
#include<ramen/util/flags.hpp>

namespace ramen
{

param_t::param_t() : QObject(), param_set_(0)
{
    flags_ = persist_bit | can_undo_bit | enabled_bit | include_in_hash_bit;
}

param_t::param_t( const std::string& name) : QObject(), param_set_(0), name_(name)
{
    flags_ = persist_bit | can_undo_bit | enabled_bit | include_in_hash_bit;
}

param_t::param_t( const param_t& other) : QObject(), param_set_(0), id_( other.id_), name_( other.name_),
                                            value_( other.value_), flags_( other.flags_)
{
}

void param_t::init()    { do_init();}
void param_t::do_init() {}

void param_t::set_param_set( param_set_t *parent)
{
    param_set_ = parent;
    do_set_param_set( parent);
}

void param_t::do_set_param_set( param_set_t *parent) {}

const parameterised_t *param_t::parameterised() const
{
    RAMEN_ASSERT( param_set());
    return param_set()->parent();
}

parameterised_t *param_t::parameterised()
{
    RAMEN_ASSERT( param_set());
    return param_set()->parent();
}

const composition_t *param_t::composition() const
{
    RAMEN_ASSERT( param_set());
    const parameterised_t *p = parameterised();

    if( p)
        return p->composition();
    else
        return 0;
}

composition_t *param_t::composition()
{
    RAMEN_ASSERT( param_set());
    parameterised_t *p = parameterised();

    if( p)
        return p->composition();
    else
        return 0;
}

void param_t::set_id( const std::string& identifier)
{
    RAMEN_ASSERT( util::is_string_valid_identifier( identifier));

    if( !util::is_string_valid_identifier( identifier))
        throw std::runtime_error( "Invalid id for param_t");

    id_ = identifier;
}

bool param_t::enabled() const { return util::test_flag( flags_, enabled_bit);}

void param_t::set_enabled( bool e)
{
    util::set_flag( flags_, enabled_bit, e);
    enable_widgets( enabled());
}

bool param_t::is_static() const      { return util::test_flag( flags_, static_bit);}

void param_t::set_static( bool b)
{
    util::set_flag( flags_, static_bit, b);
}

bool param_t::secret() const		{ return util::test_flag( flags_, secret_bit);}

void param_t::set_secret( bool b)
{
    util::set_flag( flags_, secret_bit, b);
}

bool param_t::persist() const	{ return util::test_flag( flags_, persist_bit);}

void param_t::set_persist( bool b)
{
    util::set_flag( flags_, persist_bit, b);
}

bool param_t::can_undo() const	{ return util::test_flag( flags_, can_undo_bit);}

void param_t::set_can_undo( bool b)
{
    util::set_flag( flags_, can_undo_bit, b);
}

bool param_t::include_in_hash() const { return util::test_flag( flags_, include_in_hash_bit);}

void param_t::set_include_in_hash( bool b)
{
    util::set_flag( flags_, include_in_hash_bit, b);
}

bool param_t::track_mouse() const
{
    if( const parameterised_t *p = parameterised())
    {
        if( !p->track_mouse())
            return false;
    }

    return !( flags_ & dont_track_mouse_bit);
}

void param_t::set_track_mouse( bool b)
{
    util::set_flag( flags_, dont_track_mouse_bit, b);
}

void param_t::emit_param_changed( change_reason reason)
{
    if( reason != silent_edit)
    {
        if( param_set_t *p = param_set())
            p->param_changed( this, reason);
    }
}

void param_t::format_changed( const Imath::Box2i& new_format, float aspect, const Imath::V2f& proxy_scale)
{
    do_format_changed( new_format, aspect, proxy_scale);
}

void param_t::do_format_changed( const Imath::Box2i& new_format, float aspect, const Imath::V2f& proxy_scale) {}

void param_t::add_to_hash( hash::generator_t& hash_gen) const
{
    do_add_to_hash( hash_gen);
}

void param_t::do_add_to_hash( hash::generator_t& hash_gen) const {}

void param_t::create_tracks( anim::track_t *parent)
{
    if( !is_static())
        do_create_tracks( parent);
}

void param_t::do_create_tracks( anim::track_t *parent) {}

void param_t::set_frame( float frame)
{
    evaluate( frame);
    do_set_frame( frame);
    emit_param_changed( time_changed);
}

void param_t::do_set_frame( float frame) {}

void param_t::evaluate( float frame)	{ do_evaluate( frame);}
void param_t::do_evaluate( float frame)	{}

// undo
std::auto_ptr<undo::command_t> param_t::create_command() { return do_create_command();}

std::auto_ptr<undo::command_t> param_t::do_create_command()
{
    return std::auto_ptr<undo::command_t>();
}

// paths
void param_t::convert_relative_paths( const boost::filesystem::path& old_base, const boost::filesystem::path& new_base)
{
    do_convert_relative_paths( old_base, new_base);
}

void param_t::do_convert_relative_paths( const boost::filesystem::path& old_base, const boost::filesystem::path& new_base) {}

void param_t::make_paths_absolute()     { do_make_paths_absolute();}
void param_t::do_make_paths_absolute()  {}

void param_t::make_paths_relative()     { do_make_paths_relative();}
void param_t::do_make_paths_relative()  {}

// util
void param_t::apply_function( const boost::function<void ( param_t*)>& f)
{
    f( this);
    do_apply_function( f);
}

void param_t::do_apply_function( const boost::function<void ( param_t*)>& f) {}

// serialization
void param_t::read(const serialization::yaml_node_t& in) { do_read( in);}

void param_t::do_read(const serialization::yaml_node_t& in)
{
    RAMEN_ASSERT( 0 && "We should never get here");
}

void param_t::write( serialization::yaml_oarchive_t& out) const
{
    if( !value().is_empty() && persist())
    {
        out.begin_map();
            out << YAML::Key << "id" << YAML::Value << id();
            do_write( out);
        out.end_map();
    }
}

void param_t::do_write( serialization::yaml_oarchive_t& out) const
{
    RAMEN_ASSERT( 0 && "We should never get here");
}

// widgets
QWidget *param_t::create_widgets()
{
    if( !secret())
        return do_create_widgets();

    return 0;
}
QWidget *param_t::do_create_widgets() { return 0;}

void param_t::update_widgets()      { do_update_widgets();}
void param_t::do_update_widgets()   {}

void param_t::enable_widgets( bool e)       { do_enable_widgets( e);}
void param_t::do_enable_widgets( bool e)    {}

param_t *new_clone( const param_t& other) { return other.clone();}

} // namespace
