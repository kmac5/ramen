// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_UI_VIEWER_STRATEGY_HPP
#define	RAMEN_UI_VIEWER_STRATEGY_HPP

#include<ramen/ui/viewer/viewer_strategy_fwd.hpp>

#include<boost/function.hpp>

#include<OpenEXR/ImathBox.h>
#include<OpenEXR/ImathColor.h>

#include<QObject>
#include<QKeyEvent>
#include<QMouseEvent>

#include<ramen/assert.hpp>

#include<ramen/nodes/node_fwd.hpp>
#include<ramen/ui/viewer/viewer_context_fwd.hpp>

class QWidget;

namespace ramen
{
namespace ui
{
namespace viewer
{

class viewer_strategy_t : public QObject
{
    Q_OBJECT

public:

    viewer_strategy_t( viewer_context_t *p);
    virtual ~viewer_strategy_t();

    viewer_context_t *parent()              { return parent_;}
    const viewer_context_t *parent() const  { return parent_;}

    virtual void test_gl_extensions() const {}
    virtual void init() {}
	
    virtual QWidget *toolbar();

    void begin_active_view();
    void end_active_view();

	bool is_active() const		{ return active_;}
	void set_active( bool b)	{ active_ = b;}

    // nodes
    virtual void node_added( node_t *n)     {}
    virtual void node_released( node_t *n)  {}

    virtual bool can_display_node( node_t *n) const = 0;

    virtual void set_active_node( node_t *n, bool process = false)  {}
    virtual void set_context_node( node_t *n, bool process = false) {}

    // options
    virtual void view_mode_changed()    {}
    virtual void autoupdate_changed()   {}

    // time
    virtual void frame_changed() {}

	// display
	virtual void display_transform_changed() {}
	virtual void exposure_changed() {}
	virtual void gamma_changed() {}

    // redraw
    void update();
	
    // events
    virtual void resize( int w, int h) {}
    virtual void paint();

    virtual void enter_event( QEvent *event);
    virtual void leave_event( QEvent *event);

    virtual void key_press_event( QKeyEvent *event);
    virtual void key_release_event( QKeyEvent *event);

    virtual void mouse_move_event( QMouseEvent *event);
    virtual void mouse_press_event( QMouseEvent *event);
    virtual void mouse_release_event( QMouseEvent *event);

	// eyedropper & color picking
	virtual Imath::Color4f color_at( int x, int y) const;
	virtual void pick_colors_in_box( const Imath::Box2i& b, boost::function<void ( const Imath::Color4f&)> f) const;
	
	// utils
	virtual void frame_rect( const Imath::Box2f& b) const;
	virtual void frame_rect( const Imath::Box2i& b) const;

private:

    virtual void do_begin_active_view() {}
    virtual void do_end_active_view()   {}

    viewer_context_t *parent_;
	bool active_;
};

class default_viewer_strategy_t : public viewer_strategy_t
{
    Q_OBJECT

public:

    default_viewer_strategy_t( viewer_context_t *p);

    virtual void set_active_node( node_t *n);
    virtual void set_context_node( node_t *n);

    virtual bool can_display_node( node_t *n) const;
};

} // viewer
} // ui
} // ramen

#endif
