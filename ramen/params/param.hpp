// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_PARAM_HPP
#define RAMEN_PARAM_HPP

#include<ramen/config.hpp>

#include<ramen/params/param_fwd.hpp>

#include<memory>
#include<sstream>
#include<string>
#include<vector>
#include<utility>

#include<boost/flyweight.hpp>
#include<boost/filesystem/fstream.hpp>

#include<OpenEXR/ImathBox.h>

#include<QObject>

#include<ramen/assert.hpp>

#include<ramen/app/composition_fwd.hpp>

#include<ramen/params/poly_param_value.hpp>
#include<ramen/params/parameterised_fwd.hpp>
#include<ramen/params/param_set_fwd.hpp>
#include<ramen/params/static_param_command_fwd.hpp>
#include<ramen/params/animated_param_command_fwd.hpp>

#include<ramen/anim/track_fwd.hpp>

#include<ramen/undo/command.hpp>

#include<ramen/hash/generator.hpp>

#include<ramen/serialization/yaml_iarchive.hpp>
#include<ramen/serialization/yaml_oarchive.hpp>

#include<ramen/ui/widgets/param_spinbox_fwd.hpp>

class QWidget;

namespace ramen
{

/**
\ingroup params
\brief param class
*/
class RAMEN_API param_t : public QObject
{
    Q_OBJECT

protected:

    enum flag_bits
    {
        static_bit					= 1 << 0,   // param does not animate.
        secret_bit					= 1 << 1,   // param does not show on the inspector.
        persist_bit					= 1 << 2,   // param value is not saved to file.
        can_undo_bit				= 1 << 3,   // param uses undo.
        enabled_bit					= 1 << 4,   // param widgets are enabled in the inspector.
        dont_track_mouse_bit		= 1 << 5,   // notify is called on mouse up only.
        round_to_int_bit			= 1 << 6,   // for float params, round to value to integer. Emulates an integer_param.
        proportional_bit			= 1 << 7,   // param is a float2 or float3 and the values can be modified proportionally.
        include_in_hash_bit			= 1 << 8	// param is included in hash
    };

public:

    enum change_reason
    {
        silent_edit = 0,
        user_edited,
        node_edited,
        node_loaded,
        time_changed
    };

    /// Constructor.
    param_t();

    /// Constructor.
    explicit param_t( const std::string& name);

    /// Destructor.
    virtual ~param_t() {}

    /// Post constructor initialization.
    void init();

    /// Makes a copy of this param.
    param_t *clone() const { return do_clone();}

    /// Returns the param name.
    const std::string& name() const         { return name_;}

    /// Sets the param name.
    void set_name( const std::string& name) { name_ = name;}

    /// Returns the param id.
    const std::string& id() const { return id_;}

    /// Sets the param id.
    void set_id( const std::string& identifier);

    /// Returns a const pointer to the param set this param belongs to.
    const param_set_t *param_set() const    { return param_set_;}

    /// Returns a pointer to the param set this param belongs to.
    param_set_t *param_set()				{ return param_set_;}

    /// Sets the param set this param belongs to.
    void set_param_set( param_set_t *parent);

    /// Returns a const pointer to the parameterised this param belongs to.
    const parameterised_t *parameterised() const;

    /// Returns a pointer to the parameterised this param belongs to.
    parameterised_t *parameterised();

    /// Returns a const pointer to the composition this param belongs to.
    const composition_t *composition() const;

    /// Returns a const pointer to the composition this param belongs to.
    composition_t *composition();

    // flags
    bool enabled() const;
    void set_enabled( bool e);

    bool is_static() const;
    void set_static( bool b);

    bool secret() const;
    void set_secret( bool b);

    bool persist() const;
    void set_persist( bool b);

    bool can_undo() const;
    void set_can_undo( bool b);

    bool include_in_hash() const;
    void set_include_in_hash( bool b);

    bool track_mouse() const;
    void set_track_mouse( bool b);

    // toolips
    const std::string& tooltip() const		{ return tooltip_;}
    void set_tooltip( const std::string& t) { tooltip_ = t;}

    // notifications
    void emit_param_changed( change_reason reason);

    // format
    void format_changed( const Imath::Box2i& new_format, float aspect, const Imath::V2f& proxy_scale);

    // animation
    void create_tracks( anim::track_t *parent);
    void set_frame( float frame);
    void evaluate( float frame);

    // hash
    void add_to_hash( hash::generator_t& hash_gen) const;

    // undo
    std::auto_ptr<undo::command_t> create_command();

    // paths
    void convert_relative_paths( const boost::filesystem::path& old_base, const boost::filesystem::path& new_base);
    void make_paths_absolute();
    void make_paths_relative();

    // serialization
    void read( const serialization::yaml_node_t& in);
    void write( serialization::yaml_oarchive_t& out) const;

    // util
    void apply_function( const boost::function<void ( param_t*)>& f);

    // widgets
    QWidget *create_widgets();
    void update_widgets();
    void enable_widgets( bool e);

protected:

    param_t( const param_t& other);
    void operator=( const param_t& other);

    int flags() const			{ return flags_;}
    void set_flags( int flags)	{ flags_ = flags;}

    const poly_param_value_t& value() const	{ return value_;}
    poly_param_value_t& value()             { return value_;}

    virtual poly_param_value_t value_at_frame( float frame) const { return value();}

private:

    friend class param_set_t;
    friend class static_param_command_t;
    friend class static_param_reset_command_t;
    friend class animated_param_command_t;
    friend class animated_param_reset_command_t;

    virtual param_t *do_clone() const = 0;

    virtual void do_init();

    virtual void do_format_changed( const Imath::Box2i& new_format, float aspect, const Imath::V2f& proxy_scale);

    virtual void do_set_param_set( param_set_t *parent);

    // time and anim
    virtual void do_create_tracks( anim::track_t *parent);
    virtual void do_set_frame( float frame);
    virtual void do_evaluate( float frame);

    // undo
    virtual std::auto_ptr<undo::command_t> do_create_command();

    // hash
    virtual void do_add_to_hash( hash::generator_t& hash_gen) const;

    // paths
    virtual void do_convert_relative_paths( const boost::filesystem::path& old_base, const boost::filesystem::path& new_base);
    virtual void do_make_paths_absolute();
    virtual void do_make_paths_relative();

    // serialization
    virtual void do_read( const serialization::yaml_node_t& in);
    virtual void do_write( serialization::yaml_oarchive_t& out) const;

    // util
    virtual void do_apply_function( const boost::function<void ( param_t*)>& f);

    // widgets
    virtual QWidget *do_create_widgets();
    virtual void do_update_widgets();
    virtual void do_enable_widgets( bool e);

    template<class S> friend S get_value( const param_t& p);
    template<class S> friend S get_value_at_frame( const param_t& p, float frame);

    template<class S> friend S get_absolute_value( const param_t& p);
    template<class S> friend S get_absolute_value_at_frame( const param_t& p, float frame);

    param_set_t *param_set_;

    boost::flyweight<std::string> name_;
    boost::flyweight<std::string> id_;
    boost::flyweight<std::string> tooltip_;

    boost::uint32_t flags_;
    poly_param_value_t value_;
};

template<class S>
S get_value( const param_t& p)
{
    const poly_param_value_t& any( p.value());

    #ifdef NDEBUG
        return any.cast<S>();
    #else
        try
        {
            return any.cast<S>();
        }
        catch( adobe::bad_cast& e)
        {
            RAMEN_ASSERT( 0 && "Bad cast exception in get_value");
        }
    #endif
}

template<class S>
S get_value_at_frame( const param_t& p, float frame)
{
    poly_param_value_t any( p.value_at_frame( frame));

    #ifdef NDEBUG
        return any.cast<S>();
    #else
        try
        {
            return any.cast<S>();
        }
        catch( adobe::bad_cast& e)
        {
            RAMEN_ASSERT( 0 && "Bad cast exception in get_value");
        }
    #endif
}

RAMEN_API param_t *new_clone( const param_t& other);

} // namespace

#endif
