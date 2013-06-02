// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_QUAD_MANIPULATOR_HPP
#define RAMEN_QUAD_MANIPULATOR_HPP

#include<ramen/manipulators/manipulator.hpp>

#include<boost/array.hpp>

namespace ramen
{

class float2_param_t;
	
class quad_manipulator_t : public manipulator_t
{
public:

    quad_manipulator_t( float2_param_t *topleft, float2_param_t *topright,
						float2_param_t *botleft, float2_param_t *botright);

private:
	
    virtual void do_draw_overlay( const ui::paint_event_t& event) const;

    virtual bool do_mouse_press_event( const ui::mouse_press_event_t& event);
    virtual void do_mouse_drag_event( const ui::mouse_drag_event_t& event);
    virtual void do_mouse_release_event( const ui::mouse_release_event_t& event);
	
	void get_corners( boost::array<Imath::V2f,4>& pts, float aspect, int scale = 1) const;
	
	float2_param_t *topleft_, *topright_, *botleft_, *botright_;
	int picked_corner_;
};

} // namespace

#endif
