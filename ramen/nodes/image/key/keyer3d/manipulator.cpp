// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/nodes/image/key/keyer3d/manipulator.hpp>

#include<ramen/nodes/image/key/keyer3d/keyer3d_node.hpp>
#include<ramen/nodes/image/key/keyer3d/toolbar.hpp>
#include<ramen/nodes/image/key/keyer3d/tool.hpp>

namespace ramen
{
namespace keyer3d
{

manipulator_t::manipulator_t() : ramen::manipulator_t() {}

void manipulator_t::do_draw_overlay( const ui::paint_event_t& event) const
{
	const image::keyer3d_node_t *keyer = dynamic_cast<const image::keyer3d_node_t*>( parent());	
	
	if( keyer->toolbar().active_tool())
		keyer->toolbar().active_tool()->draw_overlay( event);
}

bool manipulator_t::do_key_press_event( const ui::key_press_event_t& event)
{ 
	image::keyer3d_node_t *keyer = dynamic_cast<image::keyer3d_node_t*>( parent());

	if( keyer->toolbar().active_tool())
		keyer->toolbar().active_tool()->key_press_event( event);
	
	return true;
}

void manipulator_t::do_key_release_event( const ui::key_release_event_t& event)
{
	image::keyer3d_node_t *keyer = dynamic_cast<image::keyer3d_node_t*>( parent());

	if( keyer->toolbar().active_tool())
		keyer->toolbar().active_tool()->key_release_event( event);
}

void manipulator_t::do_mouse_enter_event( const ui::mouse_enter_event_t& event)
{
	image::keyer3d_node_t *keyer = dynamic_cast<image::keyer3d_node_t*>( parent());

	if( keyer->toolbar().active_tool())
		keyer->toolbar().active_tool()->mouse_enter_event( event);
}

void manipulator_t::do_mouse_leave_event( const ui::mouse_leave_event_t& event)
{
	image::keyer3d_node_t *keyer = dynamic_cast<image::keyer3d_node_t*>( parent());

	if( keyer->toolbar().active_tool())
		keyer->toolbar().active_tool()->mouse_leave_event( event);
}

bool manipulator_t::do_mouse_press_event( const ui::mouse_press_event_t& event)
{ 
	image::keyer3d_node_t *keyer = dynamic_cast<image::keyer3d_node_t*>( parent());
	
	if( keyer->toolbar().active_tool())
		keyer->toolbar().active_tool()->mouse_press_event( event);

	return true;
}

void manipulator_t::do_mouse_move_event( const ui::mouse_move_event_t& event)
{
	image::keyer3d_node_t *keyer = dynamic_cast<image::keyer3d_node_t*>( parent());

	if( keyer->toolbar().active_tool())
		keyer->toolbar().active_tool()->mouse_move_event( event);
}

void manipulator_t::do_mouse_drag_event( const ui::mouse_drag_event_t& event)
{
	image::keyer3d_node_t *keyer = dynamic_cast<image::keyer3d_node_t*>( parent());

	if( keyer->toolbar().active_tool())
		keyer->toolbar().active_tool()->mouse_drag_event( event);
}

void manipulator_t::do_mouse_release_event( const ui::mouse_release_event_t& event)
{
	image::keyer3d_node_t *keyer = dynamic_cast<image::keyer3d_node_t*>( parent());

	if( keyer->toolbar().active_tool())
		keyer->toolbar().active_tool()->mouse_release_event( event);
}

} // namespace
} // namespace
