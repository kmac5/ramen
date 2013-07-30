// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_COMPOSITION_VIEW_HPP
#define	RAMEN_UI_COMPOSITION_VIEW_HPP

#include<ramen/ui/compview/composition_view_fwd.hpp>



#include<boost/function.hpp>

#include<QWidget>
#include<QFont>

#include<ramen/bezier/curve.hpp>

#include<ramen/nodes/node_fwd.hpp>

#include<ramen/ui/viewport.hpp>
#include<ramen/ui/compview/composition_view_layout.hpp>
#include<ramen/ui/compview/composition_view_toolbar.hpp>
#include<ramen/ui/compview/pick_result.hpp>

class QAction;

namespace ramen
{
namespace ui
{

class composition_view_t : public QWidget
{
    Q_OBJECT

public:

    composition_view_t( QWidget *parent = 0);
	~composition_view_t();

	composition_view_toolbar_t *create_toolbar();

    Imath::V2f screen_to_world( const Imath::V2i& p) const;
    Imath::V2i world_to_screen( const Imath::V2f& p) const;

    const viewport_t& viewport() const;
	viewport_t& viewport();

    void place_node( node_t *n) const;
    void place_node_near_node( node_t *n, node_t *other) const;

    // bezier edges
    bool pick_bezier_edge( const Imath::V2f& p0, const Imath::V2f& p1, const Imath::V2f& q) const;
    void draw_bezier_edge( QPainter& painter, const Imath::V2f& p0, const Imath::V2f& p1) const;

protected:

    virtual bool event( QEvent *event);
    virtual void keyPressEvent( QKeyEvent *event);
    virtual void keyReleaseEvent( QKeyEvent *event);
    virtual void mouseDoubleClickEvent( QMouseEvent *event);
    virtual void mouseMoveEvent( QMouseEvent *event);
    virtual void mousePressEvent( QMouseEvent *event);
    virtual void mouseReleaseEvent( QMouseEvent *event);
    virtual void paintEvent ( QPaintEvent *event);
    virtual void resizeEvent( QResizeEvent *event);
	virtual void contextMenuEvent( QContextMenuEvent *event);

private:

    void scroll_drag_handler( QMouseEvent *event);
    void zoom_drag_handler( QMouseEvent *event);
	void scroll_zoom_release_handler( QMouseEvent *event);

    void move_nodes_drag_handler( QMouseEvent *event);
    void center_selected_nodes( );

    void connect_drag_handler( QMouseEvent *event);
    void connect_release_handler( QMouseEvent *event);

    void box_pick_drag_handler( QMouseEvent *event);
    void box_pick_release_handler( QMouseEvent *event);

    // draw
    void draw_edges( QPainter& p);
    void draw_nodes( QPainter& p);

    // pick
    void pick_node( const Imath::V2f& p, pick_result_t& result) const;
    bool box_pick_node( node_t *n, const Imath::Box2f& b) const;
    bool pick_edge( const Imath::V2f& p, node_t *&src, node_t *&dst, int& port) const;

    // util
    void bezier_edge( const Imath::V2f& p0, const Imath::V2f& p1, bezier::curve_t<Imath::V2f>& c) const;

	void delete_selected_nodes();
	
	composition_view_toolbar_t *toolbar_;

    bool first_resize_;
    viewport_t viewport_;
    QFont font_;

    int push_x_, push_y_;
    int last_x_, last_y_;

    bool scroll_mode_;
    bool zoom_mode_;
    Imath::V2f zoom_center_;

    bool connect_mode_;
    bool box_pick_mode_;

    pick_result_t last_pick_;
    boost::function<void ( QMouseEvent *)> drag_handler_, release_handler_;

	composition_view_layout_t layout_;
};

} // namespace
} // namespace

#endif
