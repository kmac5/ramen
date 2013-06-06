// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_MANIPULABLE_HPP
#define RAMEN_MANIPULABLE_HPP

#include<memory>

#include<boost/noncopyable.hpp>
#include<boost/ptr_container/ptr_vector.hpp>
#include<boost/signals2/signal.hpp>

#include<ramen/manipulators/manipulator.hpp>

namespace ramen
{

/*!
\ingroup manips
\brief Base class for objects that can have manipulators, react to events and draw an on-screen overlay using GL.
*/
class RAMEN_API manipulable_t : boost::noncopyable
{
public:

    manipulable_t();

    virtual ~manipulable_t();

    /// Create manipulators for this object.
    void create_manipulators();

    template<class T>
    void add_manipulator( std::auto_ptr<T> p)
    {
        p->set_parent( this);
        manipulators_.push_back( p.release());
        active_ = manipulators_.end();
    }

    const boost::ptr_vector<manipulator_t>& manipulators() const    { return manipulators_;}
    boost::ptr_vector<manipulator_t>& manipulators()                { return manipulators_;}

    typedef boost::ptr_vector<manipulator_t>::const_iterator const_iterator;
    typedef boost::ptr_vector<manipulator_t>::iterator iterator;

    const_iterator begin() const    { return manipulators_.begin();}
    const_iterator end() const	    { return manipulators_.end();}

    iterator begin()	{ return manipulators_.begin();}
    iterator end()	{ return manipulators_.end();}

    /// Emitted when the overlay needs redrawing.
    boost::signals2::signal<void ( manipulable_t*)> overlay_changed;

    /// Emits the overlay_changed signal.
    void update_overlay() { overlay_changed( this);}

    /// Called when the overlay needs redrawing.
    void draw_overlay( const ui::paint_event_t& event) const;

    /// Called when a key_press_event_t is recieved.
    bool key_press_event( const ui::key_press_event_t& event);

    /// Called when a key_release_event_t is recieved.
    void key_release_event( const ui::key_release_event_t& event);

    /// Called when a mouse_enter_event_t is recieved.
    void mouse_enter_event( const ui::mouse_enter_event_t& event);

    /// Called when a mouse_leave_event_t is recieved.
    void mouse_leave_event( const ui::mouse_leave_event_t& event);

    /// Called when a mouse_press_event_t is recieved.
    bool mouse_press_event( const ui::mouse_press_event_t& event);

    /// Called when a mouse_move_event_t is recieved.
    void mouse_move_event( const ui::mouse_move_event_t& event);

    /// Called when a mouse_drag_event_t is recieved.
    void mouse_drag_event( const ui::mouse_drag_event_t& event);

    /// Called when a mouse_release_event_t is recieved.
    void mouse_release_event( const ui::mouse_release_event_t& event);

protected:

    manipulable_t( const manipulable_t& other);
    void operator=( const manipulable_t& other);

    /*!
        \brief Customization hook for manipulable_t::draw_overlay.
        Implement in subclasses to draw a visual representation of the manipulator.
    */
    virtual void do_draw_overlay( const ui::paint_event_t& event) const;

    /*!
        \brief Customization hook for manipulable_t::key_press_event.
        Implement in subclasses if you need to handle this event class.
    */
    virtual bool do_key_press_event( const ui::key_press_event_t& event);

    /*!
        \brief Customization hook for manipulable_t::key_release_event.
        Implement in subclasses if you need to handle this event class.
    */
    virtual void do_key_release_event( const ui::key_release_event_t& event);

    /*!
        \brief Customization hook for manipulable_t::mouse_enter_event.
        Implement in subclasses if you need to handle this event class.
    */
    virtual void do_mouse_enter_event( const ui::mouse_enter_event_t& event);

    /*!
        \brief Customization hook for manipulable_t::mouse_leave_event.
        Implement in subclasses if you need to handle this event class.
    */
    virtual void do_mouse_leave_event( const ui::mouse_leave_event_t& event);

    /*!
        \brief Customization hook for manipulable_t::mouse_press_event.
        Implement in subclasses if you need to handle this event class.
    */
    virtual bool do_mouse_press_event( const ui::mouse_press_event_t& event);

    /*!
        \brief Customization hook for manipulable_t::mouse_move_event.
        Implement in subclasses if you need to handle this event class.
    */
    virtual void do_mouse_move_event( const ui::mouse_move_event_t& event);

    /*!
        \brief Customization hook for manipulable_t::mouse_drag_event.
        Implement in subclasses if you need to handle this event class.
    */
    virtual void do_mouse_drag_event( const ui::mouse_drag_event_t& event);

    /*!
        \brief Customization hook for manipulable_t::mouse_release_event.
        Implement in subclasses if you need to handle this event class.
    */    virtual void do_mouse_release_event( const ui::mouse_release_event_t& event);

private:

    /*!
    	\brief Customization hook for manipulable_t::create_manipulators.
    	Implement in subclasses add manipulators to this object.
	*/
    virtual void do_create_manipulators();

    boost::ptr_vector<manipulator_t> manipulators_;
    iterator active_;
};

} // namespace

#endif
