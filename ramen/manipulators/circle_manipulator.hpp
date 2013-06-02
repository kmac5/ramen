// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_CIRCLE_MANIPULATOR_HPP
#define RAMEN_CIRCLE_MANIPULATOR_HPP

#include<ramen/manipulators/manipulator.hpp>

namespace ramen
{

class float_param_t;
class float2_param_t;

class circle_manipulator_t : public manipulator_t
{
public:

    circle_manipulator_t( float_param_t *radius, float2_param_t *center = 0);
	
private:
	
    virtual void do_draw_overlay( const ui::paint_event_t& event) const;
    virtual bool do_mouse_press_event( const ui::mouse_press_event_t& event);
    virtual void do_mouse_drag_event( const ui::mouse_drag_event_t& event);
    virtual void do_mouse_release_event( const ui::mouse_release_event_t& event);
	
	Imath::V2f get_center() const;
	
	float2_param_t *center_;
	float_param_t *radius_;
	int selected_;
};

} // namespace

#endif
