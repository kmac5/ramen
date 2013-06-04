// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_POINT2_MANIPULATOR_HPP
#define RAMEN_POINT2_MANIPULATOR_HPP

#include<ramen/manipulators/manipulator.hpp>

namespace ramen
{

class float2_param_t;
	
class point2_manipulator_t : public manipulator_t
{
public:

    point2_manipulator_t( float2_param_t *param);
	
private:
	
    virtual void do_draw_overlay( const ui::paint_event_t& event) const;
    virtual bool do_mouse_press_event( const ui::mouse_press_event_t& event);
    virtual void do_mouse_drag_event( const ui::mouse_drag_event_t& event);
    virtual void do_mouse_release_event( const ui::mouse_release_event_t& event);
	
	float2_param_t *param_;
	bool selected_;
};

} // namespace

#endif
