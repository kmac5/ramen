// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_UI_EVENTS_UTIL_HPP
#define RAMEN_UI_EVENTS_UTIL_HPP

#include<ramen/ui/events.hpp>

class QInputEvent;
class QKeyEvent;
class QMouseEvent;

namespace ramen
{
namespace ui
{

int translate_modifiers( QInputEvent *event);
int translate_mouse_buttons( QMouseEvent *event);

key_event_t::key_t translate_keys( QKeyEvent *event);

} // namespace
} // namespace

#endif
