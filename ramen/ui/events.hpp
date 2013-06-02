// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_EVENTS_HPP
#define RAMEN_UI_EVENTS_HPP

#include<ramen/ui/events_fwd.hpp>

#include<OpenEXR/ImathVec.h>

#include<ramen/ui/viewer/viewer_strategy_fwd.hpp>

namespace ramen
{
namespace ui
{

/**
\ingroup events
\brief base class for events
*/
struct event_t
{
    enum modifiers_t
    {
		no_modifier			= 1 << 0,
        control_modifier    = 1 << 1,
        shift_modifier      = 1 << 2,
        alt_modifier        = 1 << 3,
        meta_modifier       = 1 << 4
    };

	event_t();
	
    int modifiers;
	
    viewer::viewer_strategy_t *view;
	float aspect_ratio;
	float pixel_scale;
	int subsample;
};

/**
\ingroup events
\brief base class for key events
*/
struct key_event_t : public event_t
{
    enum key_t
    {
		no_key = 0,
        enter_key,
        return_key,
        backspace_key,
        delete_key,
        escape_key,
		
		a_key,
		b_key,
		c_key,
		d_key,
		e_key,
		f_key,
		g_key,
		h_key,
		i_key,
		j_key,
		k_key,
		l_key,
		m_key,
		n_key,
		o_key,
		p_key,
		q_key,
		r_key,
		s_key,
		t_key,
		u_key,
		v_key,
		w_key,
		x_key,
		y_key,
		z_key,
		
        unknown_key
    };

	key_event_t();
	
    key_t key;
};

/**
\ingroup events
\brief key press event class
*/
struct key_press_event_t    : public key_event_t {};

/**
\ingroup events
\brief key release event class
*/
struct key_release_event_t  : public key_event_t {};

/**
\ingroup events
\brief mouse enter event class
*/
struct mouse_enter_event_t : public event_t {};

/**
\ingroup events
\brief mouse leave event class
*/
struct mouse_leave_event_t : public event_t {};

/**
\ingroup events
\brief base class for mouse events
*/
struct mouse_event_t : public event_t
{
    enum mouse_button_t
    {
        no_button       = 0,
        left_button     = 1 << 0,
        middle_button   = 1 << 1,
        right_button    = 1 << 2
    };

	mouse_event_t();

    int button;

    Imath::V2i pos;
    Imath::V2f wpos;
};

/**
\ingroup events
\brief mouse press event class
*/
struct mouse_press_event_t : public mouse_event_t {};

/**
\ingroup events
\brief mouse move event class
*/
struct mouse_move_event_t : public mouse_event_t {};

/**
\ingroup events
\brief mouse drag event class
*/
struct mouse_drag_event_t : public mouse_event_t
{
    bool first_drag;
    Imath::V2i click_pos, last_pos;
    Imath::V2f click_wpos, last_wpos;
};

/**
\ingroup events
\brief mouse release event class
*/
struct mouse_release_event_t : public mouse_event_t
{
    bool dragged;
    Imath::V2i click_pos;
    Imath::V2f click_wpos;
};

/**
\ingroup events
\brief paint event class
*/
struct paint_event_t
{
	paint_event_t();

    const viewer::viewer_strategy_t *view;
	float aspect_ratio;
	float pixel_scale;
	int subsample;
};

} // namespace
} // namespace

#endif
