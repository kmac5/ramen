// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/manipulators/manipulable.hpp>

#include<boost/bind.hpp>

#include<boost/range/algorithm/for_each.hpp>

#include<ramen/ui/events.hpp>

namespace ramen
{

manipulable_t::manipulable_t() {}

manipulable_t::manipulable_t( const manipulable_t& other) {}

manipulable_t::~manipulable_t() {}

void manipulable_t::create_manipulators()
{
    do_create_manipulators();
}

void manipulable_t::do_create_manipulators() {}

void manipulable_t::draw_overlay( const ui::paint_event_t& event) const    { do_draw_overlay( event);}
void manipulable_t::do_draw_overlay( const ui::paint_event_t& event) const
{
    boost::range::for_each( manipulators(), boost::bind( &manipulator_t::draw_overlay, _1, event));
}

bool manipulable_t::key_press_event( const ui::key_press_event_t& event) { return do_key_press_event( event);}

bool manipulable_t::do_key_press_event( const ui::key_press_event_t& event)
{
    active_ = end();

    for( iterator it( begin()); it != end(); ++it)
    {
        if( it->key_press_event( event))
        {
            active_ = it;
            return true;
        }
    }

    return false;
}

void manipulable_t::key_release_event( const ui::key_release_event_t& event) { do_key_release_event( event);}

void manipulable_t::do_key_release_event( const ui::key_release_event_t& event)
{
    if( active_ != end())
        active_->key_release_event( event);

    active_ = end();
}

void manipulable_t::mouse_enter_event( const ui::mouse_enter_event_t& event) { do_mouse_enter_event( event);}
void manipulable_t::do_mouse_enter_event( const ui::mouse_enter_event_t& event)
{
    boost::range::for_each( manipulators(), boost::bind( &manipulator_t::mouse_enter_event, _1, event));
}

void manipulable_t::mouse_leave_event( const ui::mouse_leave_event_t& event) { do_mouse_leave_event( event);}
void manipulable_t::do_mouse_leave_event( const ui::mouse_leave_event_t& event)
{
    boost::range::for_each( manipulators(), boost::bind( &manipulator_t::mouse_leave_event, _1, event));
}

bool manipulable_t::mouse_press_event( const ui::mouse_press_event_t& event)
{
    return do_mouse_press_event( event);
}

bool manipulable_t::do_mouse_press_event( const ui::mouse_press_event_t& event)
{
    active_ = end();

    for( iterator it( begin()); it != end(); ++it)
    {
        if( it->mouse_press_event( event))
        {
            active_ = it;
            return true;
        }
    }
    
    return false;
}

void manipulable_t::mouse_move_event( const ui::mouse_move_event_t& event) { do_mouse_move_event( event);}

void manipulable_t::do_mouse_move_event( const ui::mouse_move_event_t& event)
{
    boost::range::for_each( manipulators(), boost::bind( &manipulator_t::mouse_move_event, _1,  event));
}

void manipulable_t::mouse_drag_event( const ui::mouse_drag_event_t& event) { do_mouse_drag_event( event);}

void manipulable_t::do_mouse_drag_event( const ui::mouse_drag_event_t& event)
{
    if( active_ != end())
        active_->mouse_drag_event( event);
}

void manipulable_t::mouse_release_event( const ui::mouse_release_event_t& event) { do_mouse_release_event( event);}

void manipulable_t::do_mouse_release_event( const ui::mouse_release_event_t& event)
{
    if( active_ != end())
        active_->mouse_release_event( event);

    active_ = end();
}

} // namespace
