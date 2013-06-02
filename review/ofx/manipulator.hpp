// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_OFX_MANIPULATOR_HPP
#define RAMEN_OFX_MANIPULATOR_HPP

#include<ramen/manipulators/manipulator.hpp>

#include"ofxCore.h"
#include"ofxInteract.h"

#include"ofxhPropertySuite.h"
#include"ofxhInteract.h"

#include<ramen/ofx/ofx_node_fwd.hpp>

namespace ramen
{
namespace ofx
{

class image_effect_t;

class manipulator_t : public ramen::manipulator_t, public OFX::Host::Interact::Instance
{
public:

    manipulator_t( image::ofx_node_t *node, image_effect_t *effect);

    // Interact implementation
    virtual void getViewportSize( double &width, double &height) const;
    virtual void getPixelScale( double& xScale, double& yScale) const;
    virtual void getBackgroundColour( double &r, double &g, double &b) const;

    virtual OfxStatus swapBuffers();
    virtual OfxStatus redraw();

	static bool mouse_down();
	
private:

    const image::ofx_node_t *ofx_node() const;
    image::ofx_node_t *ofx_node();
	
    virtual void do_draw_overlay( const ui::paint_event_t& event) const;

    virtual bool do_key_press_event( const ui::key_press_event_t& event);
    virtual void do_key_release_event( const ui::key_release_event_t& event);

    virtual void do_mouse_enter_event( const ui::mouse_enter_event_t& event);
    virtual void do_mouse_leave_event( const ui::mouse_leave_event_t& event);

    virtual bool do_mouse_press_event( const ui::mouse_press_event_t& event);
    virtual void do_mouse_move_event( const ui::mouse_move_event_t& event);
    virtual void do_mouse_drag_event( const ui::mouse_drag_event_t& event);
    virtual void do_mouse_release_event( const ui::mouse_release_event_t& event);
	
	// util
	OfxPointD get_render_scale( const image::ofx_node_t *node, const ui::mouse_event_t& event) const;
	OfxPointI get_mouse_pos( const image::ofx_node_t *node, const ui::mouse_event_t& event) const;
	OfxPointD get_pen_pos( const image::ofx_node_t *node, const ui::mouse_event_t& event) const;
	
	static bool mouse_down_;
};

} // namespace
} // namespace

#endif
