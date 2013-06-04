// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/python/python.hpp>

#include<boost/python.hpp>
namespace bpy = boost::python;

#include<ramen/ui/events.hpp>

#include<ramen/python/util.hpp>

namespace ramen
{
namespace python
{
namespace
{
/*
bpy::list get_pos( const ui::mouse_event_t& event)				{ return vec_to_list( event.pos);}
bpy::list get_wpos( const ui::mouse_event_t& event)				{ return vec_to_list( event.wpos);}

bpy::list get_dclick_pos( const ui::mouse_drag_event_t& event)	{ return vec_to_list( event.click_pos);}
bpy::list get_dlast_pos( const ui::mouse_drag_event_t& event)	{ return vec_to_list( event.last_pos);}
bpy::list get_dclick_wpos( const ui::mouse_drag_event_t& event)	{ return vec_to_list( event.click_wpos);}
bpy::list get_dlast_wpos( const ui::mouse_drag_event_t& event)	{ return vec_to_list( event.last_wpos);}

bpy::list get_rclick_pos( const ui::mouse_release_event_t& event)	{ return vec_to_list( event.click_pos);}
bpy::list get_rclick_wpos( const ui::mouse_release_event_t& event)	{ return vec_to_list( event.click_wpos);}
*/
} // unnamed

void export_ui_events()
{
    /*
	bpy::enum_<ui::event_t::modifiers_t>( "modifiers_t")
		.value( "no_modifier", ui::event_t::no_modifier)
		.value( "control_modifier", ui::event_t::control_modifier)
		.value( "shift_modifier", ui::event_t::shift_modifier)
		.value( "alt_modifier", ui::event_t::alt_modifier)
		.value( "meta_modifier", ui::event_t::meta_modifier)
		;

	bpy::class_<ui::event_t, boost::noncopyable>( "event_t", bpy::no_init)
		.def_readonly( "modifiers", &ui::event_t::modifiers)
		.def_readonly( "aspect_ratio", &ui::event_t::aspect_ratio)
		.def_readonly( "pixel_scale", &ui::event_t::pixel_scale)
		.def_readonly( "subsample", &ui::event_t::subsample)
	   ;

	bpy::class_<ui::key_event_t, bpy::bases<ui::event_t>, boost::noncopyable>( "key_event_t", bpy::no_init)
		;

	bpy::class_<ui::key_press_event_t, bpy::bases<ui::key_event_t>, boost::noncopyable>( "key_release_event_t", bpy::no_init)
		;

	bpy::class_<ui::key_release_event_t, bpy::bases<ui::key_event_t>, boost::noncopyable>( "key_press_event_t", bpy::no_init)
		;

	bpy::class_<ui::mouse_enter_event_t, bpy::bases<ui::event_t>, boost::noncopyable>( "mouse_enter_event_t", bpy::no_init)
		;

	bpy::class_<ui::mouse_leave_event_t, bpy::bases<ui::event_t>, boost::noncopyable>( "mouse_leave_event_t", bpy::no_init)
		;

	bpy::class_<ui::mouse_event_t, bpy::bases<ui::event_t>, boost::noncopyable>( "mouse_event_t", bpy::no_init)
		.def_readonly( "button", &ui::mouse_event_t::button)

		.add_property( "pos", get_pos)
		.add_property( "wpos", get_wpos)
		;

	bpy::class_<ui::mouse_press_event_t, bpy::bases<ui::mouse_event_t>, boost::noncopyable>( "mouse_press_event_t", bpy::no_init)
		;

	bpy::class_<ui::mouse_move_event_t, bpy::bases<ui::mouse_event_t>, boost::noncopyable>( "mouse_move_event_t", bpy::no_init)
		;

	bpy::class_<ui::mouse_drag_event_t, bpy::bases<ui::mouse_event_t>, boost::noncopyable>( "mouse_frag_event_t", bpy::no_init)
		.def_readonly( "first_drag", &ui::mouse_drag_event_t::first_drag)
		.add_property( "click_pos", get_dclick_pos)
		.add_property( "last_pos", get_dlast_pos)
		.add_property( "click_wpos", get_dclick_wpos)
		.add_property( "last_wpos", get_dlast_wpos)
		;

	bpy::class_<ui::mouse_release_event_t, bpy::bases<ui::mouse_event_t>, boost::noncopyable>( "mouse_release_event_t", bpy::no_init)
		.def_readonly( "dragged", &ui::mouse_release_event_t::dragged)
		.add_property( "click_pos", get_rclick_pos)
		.add_property( "click_wpos", get_rclick_wpos)
		;

	bpy::class_<ui::paint_event_t, boost::noncopyable>( "paint_event_t", bpy::no_init)
		.def_readonly( "aspect_ratio", &ui::paint_event_t::aspect_ratio)
		.def_readonly( "pixel_scale", &ui::paint_event_t::pixel_scale)
		.def_readonly( "subsample", &ui::paint_event_t::subsample)
	   ;
       */
}
	
} // python
} // ramen
