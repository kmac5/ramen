// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_KEYER3D_MANIPULATOR_HPP
#define RAMEN_KEYER3D_MANIPULATOR_HPP

#include<ramen/manipulators/manipulator.hpp>

namespace ramen
{
namespace keyer3d
{
	
class manipulator_t : public ramen::manipulator_t
{
public:

    manipulator_t();

private:
	
    virtual void do_draw_overlay( const ui::paint_event_t& event) const;

    virtual bool do_key_press_event( const ui::key_press_event_t& event);
    virtual void do_key_release_event( const ui::key_release_event_t& event);

    virtual void do_mouse_enter_event( const ui::mouse_enter_event_t& event);
    virtual void do_mouse_leave_event( const ui::mouse_leave_event_t& event);

    virtual bool do_mouse_press_event( const ui::mouse_press_event_t& event);
    virtual void do_mouse_move_event( const ui::mouse_move_event_t& event);
    virtual void do_mouse_drag_event( const ui::mouse_drag_event_t& event);
    virtual void do_mouse_release_event( const ui::mouse_release_event_t& event);
};

} // namespace
} // namespace

#endif
