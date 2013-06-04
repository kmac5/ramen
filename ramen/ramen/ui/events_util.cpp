// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/ui/events_util.hpp>

#include<QKeyEvent>
#include<QMouseEvent>

namespace ramen
{
namespace ui
{

int translate_modifiers( QInputEvent *event)
{
    int mods = 0;
    if( event->modifiers() & Qt::ShiftModifier)     mods |= event_t::shift_modifier;
    if( event->modifiers() & Qt::ControlModifier)   mods |= event_t::control_modifier;
    if( event->modifiers() & Qt::AltModifier)       mods |= event_t::alt_modifier;
    if( event->modifiers() & Qt::MetaModifier)      mods |= event_t::meta_modifier;
    return mods;
}

int translate_mouse_buttons( QMouseEvent *event)
{
    switch( event->button())
    {
    case Qt::LeftButton:
        return mouse_event_t::left_button;

    case Qt::MidButton:
        return mouse_event_t::middle_button;

    case Qt::RightButton:
        return mouse_event_t::right_button;

    default:
        return 0;
    }
}

key_event_t::key_t translate_keys( QKeyEvent *event)
{
    switch( event->key())
    {
    case Qt::Key_Escape:
        return key_event_t::escape_key;

    case Qt::Key_Enter:
        return key_event_t::enter_key;

    case Qt::Key_Return:
        return key_event_t::return_key;

    case Qt::Key_Backspace:
        return key_event_t::backspace_key;

    case Qt::Key_Delete:
        return key_event_t::delete_key;

	case Qt::Key_A:
		return key_event_t::a_key;

	case Qt::Key_B:
		return key_event_t::b_key;
		
	case Qt::Key_C:
		return key_event_t::c_key;
		
	case Qt::Key_D:
		return key_event_t::d_key;
		
	case Qt::Key_E:
		return key_event_t::e_key;
		
	case Qt::Key_F:
		return key_event_t::f_key;
		
	case Qt::Key_G:
		return key_event_t::g_key;
		
	case Qt::Key_H:
		return key_event_t::h_key;
		
	case Qt::Key_I:
		return key_event_t::i_key;
		
	case Qt::Key_J:
		return key_event_t::j_key;
		
	case Qt::Key_K:
		return key_event_t::k_key;
		
	case Qt::Key_L:
		return key_event_t::l_key;
		
	case Qt::Key_M:
		return key_event_t::m_key;
		
	case Qt::Key_N:
		return key_event_t::n_key;
		
	case Qt::Key_O:
		return key_event_t::o_key;
		
	case Qt::Key_P:
		return key_event_t::p_key;
		
	case Qt::Key_Q:
		return key_event_t::q_key;
		
	case Qt::Key_R:
		return key_event_t::r_key;
		
	case Qt::Key_S:
		return key_event_t::s_key;
		
	case Qt::Key_T:
		return key_event_t::t_key;
		
	case Qt::Key_U:
		return key_event_t::u_key;
		
	case Qt::Key_V:
		return key_event_t::v_key;
		
	case Qt::Key_W:
		return key_event_t::w_key;
		
	case Qt::Key_X:
		return key_event_t::x_key;
		
	case Qt::Key_Y:
		return key_event_t::y_key;
		
	case Qt::Key_Z:
		return key_event_t::z_key;

    default:
        return key_event_t::unknown_key;
    }
}

} // namespace
} // namespace
