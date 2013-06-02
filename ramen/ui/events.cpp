// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/ui/events.hpp>

namespace ramen
{
namespace ui
{

event_t::event_t() : modifiers( no_modifier), view( 0)
{
	aspect_ratio = 1.0f;
	pixel_scale = 1.0f;
	subsample = 1;	
}

key_event_t::key_event_t() : key( no_key) {}

mouse_event_t::mouse_event_t() : button( no_button) {}

paint_event_t::paint_event_t() : view( 0)
{
	aspect_ratio = 1.0f;
	pixel_scale = 1.0f;
	subsample = 1;	
}

} // namespace
} // namespace
