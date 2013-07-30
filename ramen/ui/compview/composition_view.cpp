// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/compview/composition_view.hpp>

#include<sstream>
#include<iostream>

#include<boost/foreach.hpp>

#include<QKeyEvent>
#include<QMouseEvent>
#include<QPaintEvent>
#include<QResizeEvent>
#include<QHelpEvent>
#include<QPainter>
#include<QPen>
#include<QBrush>
#include<QToolTip>
#include<QAction>
#include<QContextMenuEvent>
#include<QMenu>

#include<ramen/app/application.hpp>
#include<ramen/app/document.hpp>

#include<ramen/undo/stack.hpp>

#include<ramen/bezier/algorithm.hpp>

#include<ramen/nodes/image_node.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/palette.hpp>
#include<ramen/ui/main_window.hpp>

#include<ramen/ui/compview/composition_view_commands.hpp>
#include<ramen/ui/compview/draw_pick_visitors.hpp>
#include<ramen/ui/compview/draw_pick_generic_node.hpp>

namespace ramen
{
namespace ui
{

composition_view_t::composition_view_t( QWidget *parent) : QWidget( parent)
{
    setFocusPolicy( Qt::ClickFocus);
    font_ = QFont( "Helvetica", 10);
    scroll_mode_ = false;
    first_resize_ = true;
    connect_mode_ = false;
	box_pick_mode_ = false;
    setMouseTracking( true);
	toolbar_ = 0;
}

composition_view_t::~composition_view_t() {}

composition_view_toolbar_t *composition_view_t::create_toolbar()
{
	RAMEN_ASSERT( toolbar_ == 0);

	toolbar_ = new composition_view_toolbar_t( this);
	return toolbar_;
}

const viewport_t& composition_view_t::viewport() const	{ return viewport_;}
viewport_t& composition_view_t::viewport()				{ return viewport_;}

Imath::V2f composition_view_t::screen_to_world( const Imath::V2i& p) const
{
    return viewport().screen_to_world( p);
}

Imath::V2i composition_view_t::world_to_screen( const Imath::V2f& p) const
{
    return viewport().world_to_screen( p);
}

void composition_view_t::place_node( node_t *n) const { layout_.place_node( n);}

void composition_view_t::place_node_near_node( node_t *n, node_t *other) const
{
	layout_.place_node_near_node( n, other);
}

bool composition_view_t::event( QEvent *event)
{
    if( event->type() == QEvent::ToolTip)
    {
        QHelpEvent *help_event = static_cast<QHelpEvent*>( event);

        pick_result_t picked;
        pick_node( screen_to_world( Imath::V2i( help_event->pos().x(), help_event->pos().y())), picked);

        switch( picked.component)
        {
        case pick_result_t::no_pick:
            QToolTip::hideText();
        break;

        case pick_result_t::body_picked:
        {
            node_t *node = picked.node;

            std::stringstream s;
            s << node->name();

            if( image_node_t *n = dynamic_cast<image_node_t*>( node))
                s << " [ " << n->full_format().size().x + 1 << ", " << n->full_format().size().y + 1 << "]";

            QToolTip::showText( help_event->globalPos(), QString::fromStdString( s.str()));
        }
        break;

        case pick_result_t::output_picked:
            // TODO: use the output plug's tooltip here.
            QToolTip::showText( help_event->globalPos(), "Output");
        break;

        default:
            QToolTip::showText( help_event->globalPos(), picked.node->input_plugs()[ picked.plug_num].tooltip().c_str());
        }

        return true;
    }
    else
        QWidget::event( event);
}

void composition_view_t::keyPressEvent( QKeyEvent *event)
{
    switch( event->key())
    {
		case Qt::Key_Backspace:
		case Qt::Key_Delete:
			delete_selected_nodes();
		break;
	
		case Qt::Key_Home:
			viewport().reset();
			layout_.set_world( viewport().world());
			update();
			event->accept();
		break;
	
		case Qt::Key_Comma:
		{
			Imath::V2f p( screen_to_world( viewport().device().center()));
			viewport().zoom( p, 1.33f);
			layout_.set_world( viewport().world());
			update();
			event->accept();
		}
		break;
	
		case Qt::Key_Period:
		{
			Imath::V2f p( screen_to_world( viewport().device().center()));
			viewport().zoom( p, 0.66f);
			layout_.set_world( viewport().world());
			update();
			event->accept();
		}
		break;
	
		case Qt::Key_C:
		{
			center_selected_nodes();
			update();
			event->accept();
		}
		break;
	
		default:
			event->ignore();
    }
}

void composition_view_t::keyReleaseEvent( QKeyEvent *event)
{ 
    switch( event->key())
    {
		case Qt::Key_Backspace:
		case Qt::Key_Delete:
		case Qt::Key_Home:
		case Qt::Key_Comma:
		case Qt::Key_Period:
			event->accept();
		break;
	
		default:
			event->ignore();
    }
}

void composition_view_t::mouseDoubleClickEvent( QMouseEvent *event)
{
	Imath::V2f wpos = screen_to_world( Imath::V2i( event->x(), event->y()));
	layout_.set_interest_point( wpos);

    if( last_pick_.component == pick_result_t::body_picked)
    {
        if( event->modifiers() & Qt::ControlModifier)
            app().ui()->set_context_node( last_pick_.node);
        else
            app().ui()->set_active_node( last_pick_.node);

        app().ui()->update();
    }

    event->accept();
}

void composition_view_t::mousePressEvent( QMouseEvent *event)
{	
    event->accept();

    drag_handler_    = boost::function<void ( QMouseEvent *)>();
    release_handler_ = boost::function<void ( QMouseEvent *)>();
    connect_mode_ = false;
    box_pick_mode_ = false;

    push_x_ = event->x();
    push_y_ = event->y();
    last_x_ = event->x();
    last_y_ = event->y();

    scroll_mode_ = false;
    zoom_mode_ = false;
	
	Imath::V2f wpos = screen_to_world( Imath::V2i( push_x_, push_y_));

    if( event->modifiers() & Qt::AltModifier)
    {
        if( event->modifiers() & Qt::ShiftModifier)
        {
            zoom_mode_ = true;
            zoom_center_ = wpos;
            drag_handler_ = boost::bind( &composition_view_t::zoom_drag_handler, this, _1);
			release_handler_ = boost::bind( &composition_view_t::scroll_zoom_release_handler, this, _1);
        }
		else
		{
	        scroll_mode_ = true;
		    drag_handler_ = boost::bind( &composition_view_t::scroll_drag_handler, this, _1);
			release_handler_ = boost::bind( &composition_view_t::scroll_zoom_release_handler, this, _1);
		}
        return;
    }

    pick_node( wpos, last_pick_);

    if( event->modifiers() & Qt::ControlModifier)
    {
        node_t *src = 0;
        node_t *dst = 0;
        int port = -1;

        if( pick_edge( wpos, src, dst, port))
        {
            // TODO: if either src or dest are groups, resolve the real nodes.
            std::auto_ptr<undo::command_t> c( new undo::disconnect_command_t( src, dst, port));
            c->redo();
            app().document().undo_stack().push_back( c);
            app().ui()->update();
            return;
        }
    }

    if( (last_pick_.component != pick_result_t::no_pick) && ( last_pick_.component != pick_result_t::body_picked))
    {
        connect_mode_ = true;
        drag_handler_	 = boost::bind( &composition_view_t::connect_drag_handler, this, _1);
        release_handler_ = boost::bind( &composition_view_t::connect_release_handler, this, _1);
    }

    // single push
    if( last_pick_.component == pick_result_t::no_pick && !( event->modifiers() & Qt::ShiftModifier))
    {
        app().document().composition().deselect_all();
        box_pick_mode_ = true;
        drag_handler_	 = boost::bind( &composition_view_t::box_pick_drag_handler, this, _1);
        release_handler_ = boost::bind( &composition_view_t::box_pick_release_handler, this, _1);
    }
    else
    {
        if( last_pick_.component == pick_result_t::body_picked)
        {
            if( !last_pick_.node->selected())
            {
                if( !( event->modifiers() & Qt::ShiftModifier))
                    app().document().composition().deselect_all();

                last_pick_.node->toggle_selection();
                drag_handler_ = boost::bind( &composition_view_t::move_nodes_drag_handler, this, _1);
            }
            else
            {
                if( event->modifiers() & Qt::ShiftModifier)
                    last_pick_.node->select( false);
                else
                    drag_handler_ = boost::bind( &composition_view_t::move_nodes_drag_handler, this, _1);
            }
        }
    }

    app().ui()->update();
}

void composition_view_t::mouseMoveEvent( QMouseEvent *event)
{
    if( !event->buttons())
        return;

    if( ( event->x() != last_x_) || ( event->y() != last_y_))
    {
        if( drag_handler_)
            drag_handler_( event);

        last_x_ = event->x();
        last_y_ = event->y();
        event->accept();
    }
}

void composition_view_t::mouseReleaseEvent( QMouseEvent *event)
{
	Imath::V2f wpos = screen_to_world( Imath::V2i( event->x(), event->y()));
	layout_.set_interest_point( wpos);

    if( release_handler_)
        release_handler_( event);

    scroll_mode_ = false;
    zoom_mode_ = false;
    event->accept();
}

void composition_view_t::scroll_drag_handler( QMouseEvent *event)
{
    viewport().scroll( Imath::V2i( -(event->x() - last_x_), -(event->y() - last_y_)));
    update();
}

void composition_view_t::zoom_drag_handler( QMouseEvent *event)
{
    const float zoom_speed = 0.05f;
    float zoom = 1.0f + ( zoom_speed * ( event->x() - last_x_));
    viewport().zoom( zoom_center_, zoom, zoom);
    update();
}

void composition_view_t::scroll_zoom_release_handler( QMouseEvent *event)
{
	layout_.set_world( viewport().world());
}

void composition_view_t::move_nodes_drag_handler( QMouseEvent *event)
{
    float xoffset = ( event->x() - last_x_) / ( width()  / viewport().world().size().x);
    float yoffset = ( event->y() - last_y_) / ( height() / viewport().world().size().y);
    Imath::V2f offset( xoffset, yoffset);

    for( composition_t::node_iterator it( app().document().composition().nodes().begin());
		    it != app().document().composition().nodes().end(); ++it)
    {
        if( it->selected())
            it->offset_location( offset);
    }

    update();
}

void composition_view_t::center_selected_nodes()
{
	int count = 0;
	float max_x;
	float max_y;
	float min_x;
	float min_y;

    for( composition_t::node_iterator it( app().document().composition().nodes().begin());
		    it != app().document().composition().nodes().end(); ++it)
    {
		// If no nodes are selected center all nodes
        if( it->selected() || !app().document().composition().any_selected())
		{
			if( count == 0)
			{
				min_x = it->location()[0];
				min_y = it->location()[1];
				max_x = it->location()[0];
				max_y = it->location()[1];
				count++;
			}
			if( it->location()[0] < min_x)
				min_x = it->location()[0];
			if( it->location()[1] < min_y)
				min_y = it->location()[1];
			if( it->location()[0] + generic_node_width(&(*it)) > max_x)
				max_x = it->location()[0] + generic_node_width(&(*it));
			if( it->location()[1] + generic_node_height() > max_y)
				max_y = it->location()[1] + generic_node_height();
		}
    }
	
	Imath::V2f q( ( max_x - min_x)/2 + min_x, ( max_y - min_y)/2 + min_y);
	viewport_.scroll_to_center_point( q);
}

void composition_view_t::connect_drag_handler( QMouseEvent *event) { update();}

void composition_view_t::connect_release_handler( QMouseEvent *event)
{
    connect_mode_ = false;

    Imath::V2f p( screen_to_world( Imath::V2i( last_x_, last_y_)));

    pick_result_t dest;
    pick_node( p, dest);

    if( dest.component == pick_result_t::no_pick || dest.component == pick_result_t::body_picked)
    {
        update();
        return;
    }

    node_t *src = 0;
    node_t *dst = 0;
    int port = -1;

    if( last_pick_.component == pick_result_t::output_picked)
    {
        if( dest.component == pick_result_t::output_picked)
        {
            update();
            return;
        }

        src = last_pick_.node;
        dst = dest.node;
        port = dest.plug_num;
    }
    else
    {
        if( dest.component == pick_result_t::input_picked)
        {
            update();
            return;
        }

        dst = last_pick_.node;
        src = dest.node;
        port = last_pick_.plug_num;
    }

    // TODO: if either source or dest are groups, resolve the real nodes here.
    if( app().document().composition().can_connect( src, dst, port))
    {
        std::auto_ptr<undo::command_t> c( new undo::connect_command_t( src, dst, port));
        c->redo();
        app().document().undo_stack().push_back( c);
        app().ui()->update();
    }
    else
        update();
}

void composition_view_t::box_pick_drag_handler( QMouseEvent *event) { update();}

void composition_view_t::box_pick_release_handler( QMouseEvent *event)
{
    Imath::Box2f b( screen_to_world( Imath::V2i( push_x_, push_y_)));
    b.extendBy( screen_to_world( Imath::V2i( last_x_, last_y_)));

    BOOST_FOREACH( node_t& n, app().document().composition().nodes())
    {
        if( box_pick_node( &n, b))
            n.toggle_selection();
    }

    box_pick_mode_ = false;
    app().ui()->update();
}

void composition_view_t::resizeEvent( QResizeEvent *event)
{
    if( first_resize_)
    {
		if( event->size().width() == 0 || event->size().height() == 0)
		{
			event->accept();
			return;
		}

        first_resize_ = false;
        viewport().reset( event->size().width(), event->size().height());
    }
	else
	    viewport().resize( event->size().width(), event->size().height());

	event->accept();
	layout_.set_world( viewport().world());
}

void composition_view_t::paintEvent ( QPaintEvent *event)
{
    QPainter p( this);
    p.setRenderHint( QPainter::Antialiasing);
    p.setFont( font_);
    p.setPen( Qt::NoPen);

    QBrush brush;
    brush.setColor( palette_t::instance().qcolor( "background"));
    brush.setStyle( Qt::SolidPattern);
    p.setBrush( brush);
    p.drawRect( 0, 0, width(), height());

    QTransform xf;
    xf.translate( viewport().device().min.x, viewport().device().min.y);
    xf.scale( viewport().zoom_x(), viewport().zoom_y());
    xf.translate( -viewport().world().min.x, -viewport().world().min.y);
    p.setWorldTransform( xf);

    p.setBrush( Qt::NoBrush);
    QPen pen;
    pen.setColor( palette_t::instance().qcolor( "text"));
    p.setPen( pen);

    draw_edges( p);
    draw_nodes( p);

    p.setBrush( Qt::NoBrush);

    if( connect_mode_)
    {
		pen.setColor( palette_t::instance().qcolor( "text"));
        p.setPen( pen);
        Imath::V2f q0( screen_to_world( Imath::V2i( push_x_, push_y_)));
        Imath::V2f q1( screen_to_world( Imath::V2i( last_x_, last_y_)));
        draw_bezier_edge( p, q0, q1);
    }
    else
    {
        if( box_pick_mode_)
        {
            p.resetTransform();
			pen.setColor( palette_t::instance().qcolor( "box_pick"));
            p.setPen( pen);
            p.drawLine( push_x_, push_y_, last_x_, push_y_);
            p.drawLine( last_x_, push_y_, last_x_, last_y_);
            p.drawLine( last_x_, last_y_, push_x_, last_y_);
            p.drawLine( push_x_, last_y_, push_x_, push_y_);
        }
    }

    event->accept();
}

void composition_view_t::draw_edges( QPainter& p)
{
    draw_edges_visitor visitor( *this, p);

    BOOST_FOREACH( node_t& n, app().document().composition().nodes())
        n.accept( visitor);
}

void composition_view_t::draw_nodes( QPainter& p)
{
    draw_node_visitor visitor( p);

    BOOST_FOREACH( node_t& n, app().document().composition().nodes())
        n.accept( visitor);
}

void composition_view_t::draw_bezier_edge( QPainter& painter, const Imath::V2f& p0, const Imath::V2f& p1) const
{
    bezier::curve_t<Imath::V2f> c;
    bezier_edge( p0, p1, c);

    QPainterPath path;
    path.moveTo( c[0].x, c[0].y);
    path.cubicTo( c[1].x, c[1].y, c[2].x, c[2].y, c[3].x, c[3].y);
    painter.drawPath( path);
}

// pick
void composition_view_t::pick_node( const Imath::V2f& p, pick_result_t& result) const
{
    result.node = 0;
    result.component = pick_result_t::no_pick;
    result.plug_num = -1;

    pick_node_visitor visitor( *this, p, result);

	composition_t::reverse_node_iterator it( app().document().composition().nodes().rbegin());
	composition_t::reverse_node_iterator last( app().document().composition().nodes().rend());

	for( ; it != last; ++it)
    {
        it->accept( visitor);

		if( result.component != pick_result_t::no_pick)
			break;
    }
}

bool composition_view_t::box_pick_node( node_t *n, const Imath::Box2f& b) const
{
    box_pick_node_visitor visitor( b);
	n->accept( visitor);
    return visitor.result;
}

bool composition_view_t::pick_edge( const Imath::V2f& p, node_t *&src, node_t *&dst, int& port) const
{
    pick_edge_visitor visitor( *this, p);

    BOOST_FOREACH( node_t& n, app().document().composition().nodes())
    {
        n.accept( visitor);

        if( visitor.src != 0)
        {
            src = visitor.src;
            dst = visitor.dst;
            port = visitor.port;
            return true;
        }
    }

    return false;
}

bool composition_view_t::pick_bezier_edge( const Imath::V2f& p0, const Imath::V2f& p1, const Imath::V2f& q) const
{
    bezier::curve_t<Imath::V2f> c;
    bezier_edge( p0, p1, c);

    Imath::Box2f box( c.bounding_box());

    float dist_tol = 5 / viewport().zoom_x();

    // grow the box just a bit, depending on the viewport scale
    box.min.x -= dist_tol;
    box.min.y -= dist_tol;
    box.max.x += dist_tol;
    box.max.y += dist_tol;

    if( !box.intersects( q))
        return false;

    Imath::V2f nearest = bezier::nearest_point_on_curve( c, q);
    float dist2 = ( nearest - q).length2();

    return dist2 < ( dist_tol * dist_tol);
}

// util
void composition_view_t::bezier_edge( const Imath::V2f& p0, const Imath::V2f& p1, bezier::curve_t<Imath::V2f>& c) const
{
    const float tangent_offset = 40;

    c[0] = p0;
    c[3] = p1;

    c[1] = p0;
    c[1].y += tangent_offset;

    c[2] = p1;
    c[2].y -= tangent_offset;
}

void composition_view_t::contextMenuEvent( QContextMenuEvent *event) { event->accept();}

void composition_view_t::delete_selected_nodes()
{
	app().ui()->main_window()->delete_nodes();
}

} // namespace
} // namespace
