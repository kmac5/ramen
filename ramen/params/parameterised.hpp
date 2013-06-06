// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_PARAMETERISED_HPP
#define	RAMEN_PARAMETERISED_HPP

#include<ramen/manipulators/manipulable.hpp>

#include<utility>

#include<boost/noncopyable.hpp>
#include<boost/signals2/signal.hpp>

#include<OpenEXR/ImathBox.h>

#include<ramen/params/param_set.hpp>

#include<ramen/nodes/node_fwd.hpp>

#include<ramen/app/composition_fwd.hpp>

#include<ramen/anim/track_fwd.hpp>

namespace ramen
{

/**
\ingroup params
\brief Base class for objects that contains params.
*/
class RAMEN_API parameterised_t : public manipulable_t
{
public:

    /// Constructor.
    parameterised_t();

    /// Makes a copy of the parameterised.
    parameterised_t *clone() const;

    /// Called for the new parameterised, after being copied.
    virtual void cloned() {}

    /// Destructor.
    virtual ~parameterised_t();

    /// Emitted when this object is deleted.
    boost::signals2::signal<void ( parameterised_t*)> deleted;

    /// Returns the parameterised name.
    const std::string& name() const			{ return name_;}

    /// Sets the parameterised name.
    void set_name( const std::string& n);

    bool dont_persist_params() const        { return dont_persist_params_;}
    void set_dont_persist_params( bool b)   { dont_persist_params_ = b;}

    virtual bool autokey() const;
    virtual bool track_mouse() const;

    /// Returns the parameterised parent.
    const parameterised_t *parent() const { return parent_;}

    /// Returns the parameterised parent.
    parameterised_t *parent() { return parent_;}

    /// Sets the parameterised parent.
    void set_parent( parameterised_t *parent);

    /// Returns the composition this parameterised belongs to.
    const composition_t *composition() const;

    /// Returns the composition this parameterised belongs to.
    composition_t *composition();

    /// Returns the node this parameterised belongs to.
    const node_t *node() const;

    /// Returns the node this parameterised belongs to.
    node_t *node();

    /// Returns the world node this parameterised belongs to.
    const world_node_t *world() const;

    /// Returns the world node this parameterised belongs to.
    world_node_t *world();

    /// Creates the params for this parameterised.
    void create_params();

    /// Returns a const reference to the parameterised param_set.
    const param_set_t& param_set() const    { return params_;}

    /// Returns a reference to the parameterised param_set.
    param_set_t& param_set()				{ return params_;}

    /// Returns a const reference to the param with identifier id.
    const param_t& param( const std::string& identifier) const;

    /// Returns a reference to the param with identifier id.
    param_t& param( const std::string& identifier);

    /// Adds a param to this parameterised.
    template<class T>
    void add_param( std::auto_ptr<T> p) { param_set().add_param( p);}

    /// Called after editing of params finished.
    virtual void param_edit_finished() = 0;

    /// Calls a function f for each param.
    virtual void for_each_param( const boost::function<void ( param_t*)>& f);

    /// Creates anim tracks for this parameterised and adds them to root.
    void create_tracks( anim::track_t *root);

    /// Sets the current frame to f.
    void set_frame( float f);

    /// Updates widgets associated with this parameterised's params.
    void update_widgets();

protected:

    parameterised_t( const parameterised_t& other);
    void operator=( const parameterised_t& other);

    /// Evaluate all params at frame frame.
    void evaluate_params( float frame);

private:

    /*!
        \brief Customization hook for parameterised_t::clone.
        For subclasses to implement.
    */
    virtual parameterised_t *do_clone() const = 0;

    /*!
        \brief Customization hook for parameterised_t::set_parent.
        For subclasses to implement.
    */
    virtual void do_set_parent( parameterised_t *parent);

    /*!
        \brief Customization hook for parameterised_t::create_params.
        For subclasses to implement.
    */
    virtual void do_create_params() {}

    /*!
        \brief Customization hook for parameterised_t::create_tracks.
        For subclasses to implement.
    */
    virtual void do_create_tracks( anim::track_t *parent) {}

    /*!
        \brief Customization hook for parameterised_t::set_frame.
        For subclasses to implement.
    */
    virtual void do_set_frame( float t) {}

    /*!
        \brief Customization hook for parameterised_t::update_widgets.
        For subclasses to implement.
    */
    virtual void do_update_widgets() {}

    std::string name_;
    parameterised_t *parent_;
    param_set_t params_;
    bool dont_persist_params_;
};

/// Makes a copy of a parameterised
RAMEN_API parameterised_t *new_clone( const parameterised_t& other);

} // namespace

#endif
