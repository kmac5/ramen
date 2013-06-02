// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/python/python.hpp>

#include<ramen/ui/anim/anim_curves_view.hpp>

#include<algorithm>
#include<cmath>

#include<boost/foreach.hpp>
#include<boost/bind.hpp>

#include<adobe/algorithm/clamp.hpp>

#include<QTreeView>
#include<QKeyEvent>
#include<QMouseEvent>
#include<QPaintEvent>
#include<QResizeEvent>
#include<QPainter>
#include<QPen>
#include<QBrush>
#include<QAction>
#include<QContextMenuEvent>
#include<QMenu>

#include<ramen/app/application.hpp>
#include<ramen/app/document.hpp>

#include<ramen/anim/track.hpp>

#include<ramen/ui/user_interface.hpp>

#include<ramen/ui/anim/anim_editor.hpp>
#include<ramen/ui/anim/anim_editor_toolbar.hpp>
#include<ramen/ui/anim/draw_curves_visitors.hpp>
#include<ramen/ui/palette.hpp>

#include<iostream>

namespace ramen
{
namespace ui
{

anim_curves_view_t::anim_curves_view_t( QWidget *parent) : QWidget( parent), first_resize_( true)
{
    setFocusPolicy( Qt::ClickFocus);
    viewport_.set_y_down( true);
    scroll_mode_ = false;
	handle_mouse_event_ = false;
	snap_ = false;
	show_tangents_ = true;
	
	frame_selected_ = new QAction( "Frame Selected", this);
	connect( frame_selected_, SIGNAL( triggered()), this, SLOT( frame_selection()));
	
	frame_all_ = new QAction( "Frame All", this);
	connect( frame_all_, SIGNAL( triggered()), this, SLOT( frame_all()));
	
	snap_frames_ = new QAction( "Snap Frames", this);
	snap_frames_->setCheckable( true);
	snap_frames_->setChecked( false);
	connect( snap_frames_, SIGNAL( triggered( bool)), this, SLOT( set_snap_frames( bool)));

	show_tans_ = new QAction( "Show Tangents", this);
	show_tans_->setCheckable( true);
	show_tans_->setChecked( true);
	connect( show_tans_, SIGNAL( triggered( bool)), this, SLOT( set_show_tangents( bool)));
}

float anim_curves_view_t::time_scale() const	{ return viewport_.zoom_x();}
float anim_curves_view_t::value_scale() const	{ return viewport_.zoom_y();}

Imath::V2i anim_curves_view_t::world_to_screen( const Imath::V2f& p) const
{ 
    return viewport_.world_to_screen( p);
}

Imath::V2f anim_curves_view_t::screen_to_world( const Imath::V2i& p) const
{
    return viewport_.screen_to_world( p);
}

void anim_curves_view_t::keyPressEvent( QKeyEvent *event)
{
    switch( event->key())
    {
    case Qt::Key_Home:
        reset_view();
        update();
        event->accept();
    break;

    case Qt::Key_Comma:
    {
        Imath::V2f p( viewport_.screen_to_world( viewport_.device().center()));
        viewport_.zoom( p, 1.33f);
        update();
        event->accept();
    }
    break;

    case Qt::Key_Period:
    {
        Imath::V2f p( viewport_.screen_to_world( viewport_.device().center()));
        viewport_.zoom( p, 0.66f);
        update();
        event->accept();
    }
    break;

    case Qt::Key_Backspace:
    case Qt::Key_Delete:
		app().ui()->anim_editor().delete_selected_keyframes();
		event->accept();
    break;
	
    default:
        app().ui()->anim_editor().toolbar().tool()->key_press_event( *this, event);
    break;
    }
}

void anim_curves_view_t::keyReleaseEvent( QKeyEvent *event)
{ 
    switch( event->key())
    {
    case Qt::Key_Home:
    case Qt::Key_Comma:
    case Qt::Key_Period:
    case Qt::Key_Backspace:
    case Qt::Key_Delete:
        event->accept();
    break;

    default:
        app().ui()->anim_editor().toolbar().tool()->key_release_event( *this, event);
    break;
    }
}

void anim_curves_view_t::mousePressEvent( QMouseEvent *event)
{
	if( event->button() != Qt::LeftButton)
	{
		handle_mouse_event_ = false;
		QWidget::mousePressEvent( event);
		return;
	}

	handle_mouse_event_ = true;
    push_x_ = event->x();
    push_y_ = event->y();
    last_x_ = event->x();
    last_y_ = event->y();

    scroll_mode_ = false;
    zoom_mode_ = false;
    zoom_center_ = screen_to_world( Imath::V2i( push_x_, push_y_));
    move_time_mode_ = false;

    if( event->modifiers() & Qt::AltModifier)
    {
        if( event->modifiers() & Qt::ShiftModifier)
        {
            zoom_mode_ = true;
            zoom_center_ = screen_to_world( Imath::V2i( push_x_, push_y_));
        }
        else
            scroll_mode_ = true;

        event->accept();
		return;
    }

	float time = app().document().composition().frame();
	Imath::V2i q( world_to_screen( Imath::V2f( time, 0)));

	if( abs( q.x - push_x_) <= 4)
	{
		move_time_mode_ = true;
        event->accept();
	}
	else
	{
		app().ui()->begin_interaction();
		app().ui()->anim_editor().toolbar().tool()->mouse_press_event( *this, event);
    }
}

void anim_curves_view_t::mouseMoveEvent( QMouseEvent *event)
{
	if( !handle_mouse_event_)
	{
		QWidget::mouseMoveEvent( event);
		return;
	}
	
    if( ( event->x() != last_x_) || ( event->y() != last_y_))
    {
        if( scroll_mode_)
        {
            viewport_.scroll( Imath::V2i( -(event->x() - last_x_), -(event->y() - last_y_)));
            update();
            event->accept();
        }
        else
        {
            if( zoom_mode_)
            {
                const float zoom_speed = 0.05f;
                float zoomx = 1.0f + ( zoom_speed * ( event->x() - last_x_));
                float zoomy = 1.0f + ( zoom_speed * ( event->y() - last_y_));

                viewport_.zoom( zoom_center_, zoomx, zoomy);
                update();
                event->accept();
            }
            else
            {
                if( move_time_mode_)
                {
                    Imath::V2i p( event->x(), event->y());
                    Imath::V2f q( screen_to_world( p));

                    if( q.x < app().ui()->start_frame())
                        q.x = app().ui()->start_frame();

                    if( q.x > app().ui()->end_frame())
                        q.x = app().ui()->end_frame();

                    if( q.x != app().ui()->frame())
                        app().ui()->set_frame( q.x);
                }
                else
                    app().ui()->anim_editor().toolbar().tool()->mouse_drag_event( *this, event);
            }
        }

        last_x_ = event->x();
        last_y_ = event->y();
    }
}

void anim_curves_view_t::mouseReleaseEvent( QMouseEvent *event)
{
	if( !handle_mouse_event_)
	{
		QWidget::mouseReleaseEvent( event);
		return;
	}
	
    if( !scroll_mode_ && !zoom_mode_ && !move_time_mode_)
    {
        app().ui()->anim_editor().toolbar().tool()->mouse_release_event( *this, event);
        app().ui()->end_interaction();
    }
    else
        event->accept();

    scroll_mode_ = false;
    zoom_mode_ = false;
    move_time_mode_ = false;
}

void anim_curves_view_t::paintEvent( QPaintEvent *event)
{
    QPainter painter( this);
    painter_ = &painter; // save the ptr for use in members
    painter.setRenderHint( QPainter::Antialiasing);
    painter.setPen( Qt::NoPen);

    // draw bg
    QBrush brush;
    brush.setColor( palette_t::instance().qcolor( "background"));
    brush.setStyle( Qt::SolidPattern);
    painter.setBrush( brush);
    painter.drawRect( 0, 0, width(), height());

    draw_grid();

    // draw in world space
    Imath::M33f view_xform_ = viewport_.world_to_screen_matrix();
    QMatrix qm( view_xform_[0][0], view_xform_[0][1], view_xform_[1][0], view_xform_[1][1], view_xform_[2][0], view_xform_[2][1]);
    painter.setWorldTransform( QTransform( qm));

    QPen pen;

    // draw curves
    draw_curve_visitor v( *this);

    BOOST_FOREACH( const anim::track_t *t, app().ui()->anim_editor().active_tracks())
    {
        pen.setColor( QColor( t->color().x, t->color().y, t->color().z));
        painter.setPen( pen);
        boost::apply_visitor( v, t->curve().get());
    }

    // draw in screen space
    painter.setWorldTransform( QTransform());

	pen.setColor( palette_t::instance().qcolor( "text"));
    painter.setPen( pen);

    draw_keyframes_visitor v2( *this, show_tangents());
    BOOST_FOREACH( const anim::track_t *t, app().ui()->anim_editor().active_tracks())
        boost::apply_visitor( v2, t->curve().get());
    
    draw_axes();
    draw_time_bar();

    app().ui()->anim_editor().toolbar().tool()->draw_overlay( *this);

    event->accept();
    painter_ = 0;
}

void anim_curves_view_t::resizeEvent( QResizeEvent *event)
{
    if( first_resize_)
    {
        reset_view();
        first_resize_ = false;
        return;
    }

    viewport_.resize( event->size().width(), event->size().height());
    event->accept();
}

void anim_curves_view_t::contextMenuEvent( QContextMenuEvent *event)
{
	QMenu menu( this);
	menu.addAction( frame_selected_);
	menu.addAction( frame_all_);
	menu.addSeparator();
	
	snap_frames_->setChecked( snap_);
	menu.addAction( snap_frames_);
	menu.addAction( show_tans_);

	menu.exec(event->globalPos());
	event->accept();
}

void anim_curves_view_t::reset_view()
{
    int start_frame = app().document().composition().start_frame();
    int end_frame = app().document().composition().end_frame();

    viewport_.reset( Imath::Box2i( Imath::V2i( 0, 0), Imath::V2i( width()-1, height()-1)),
                     Imath::Box2f( Imath::V2i( start_frame - 5, 0) , Imath::V2i( end_frame + 5, 10)));
}

void anim_curves_view_t::draw_grid() const
{
    const int spacing = 30;

    QPen pen;
    pen.setColor( palette_t::instance().qcolor( "grid"));
    pen.setWidth( 1);
    painter_->setPen( pen);

    // vertical
    {
        int nticks = std::floor( (double) width() / spacing);
        double range = nice_num( viewport_.world().max.x - viewport_.world().min.x, 0);
        double d = nice_num(range/( nticks-1), 1);
        double graphmin = std::floor( (double) viewport_.world().min.x / d) * d;
        double graphmax = std::ceil((double)  viewport_.world().max.x / d) * d;

        for( double x = graphmin; x < graphmax + 0.5 * d; x += d)
        {
            Imath::V2f p( x, 0);
            Imath::V2i q( world_to_screen( p));
            painter_->drawLine( QPointF( q.x, 0), QPointF( q.x, height()));
        }
    }

    // horizontal
    {
        int nticks = std::floor( (double) height() / spacing);
        double range = nice_num( viewport_.world().max.y - viewport_.world().min.y, 0);
        double d = nice_num(range/( nticks-1), 1);
        double graphmin = std::floor( (double) viewport_.world().min.y / d) * d;
        double graphmax = std::ceil( (double) viewport_.world().max.y / d) * d;
        double nfrac = std::max( -std::floor( std::log10( (double) d)), 0.0);

        for( double y = graphmin; y < graphmax + 0.5 * d; y += d)
        {
            Imath::V2f p( 0, y);
            Imath::V2i q( world_to_screen( p));
            painter_->drawLine( QPointF( 0, q.y), QPointF( width(), q.y));
        }
    }
}

void anim_curves_view_t::draw_axes() const
{
    const int spacing = 30;

    QPen pen;
    pen.setColor( QColor( 0, 0, 0));
    pen.setWidth( 1);
    painter_->setPen( pen);

    // horizontal
    {
        painter_->drawLine( QPointF( 0, height() - spacing), QPointF( width(), height() - spacing));

        int nticks = std::floor( (double) width() / spacing);
        double range = nice_num( viewport_.world().max.x - viewport_.world().min.x, 0);
        double d = nice_num(range/( nticks-1), 1);
        double graphmin = std::floor( (double) viewport_.world().min.x / d) * d;
        double graphmax = std::ceil( (double) viewport_.world().max.x / d) * d;

        for( double x = graphmin; x < graphmax + 0.5 * d; x += d)
        {
            Imath::V2f p( x, 0);
            Imath::V2i q( world_to_screen( p));
            painter_->drawLine( QPointF( q.x, height() - spacing - 3), QPointF( q.x, height() - spacing + 3));
            painter_->drawText( QPoint( q.x, height() - 10), QString::number( x));
        }
    }

    // vertical
    {
        painter_->drawLine( QPointF( spacing * 2, 0), QPointF( spacing * 2, height()));

        int nticks = std::floor( (double) height() / spacing);
        double range = nice_num( viewport_.world().max.y - viewport_.world().min.y, 0);
        double d = nice_num(range/( nticks-1), 1);
        double graphmin = std::floor( (double) viewport_.world().min.y / d) * d;
        double graphmax = std::ceil( (double) viewport_.world().max.y / d) * d;

        for( double y = graphmin; y < graphmax + 0.5 * d; y += d)
        {
            Imath::V2f p( 0, y);
            Imath::V2i q( world_to_screen( p));
            painter_->drawLine( QPointF( spacing * 2 - 3, q.y), QPointF( spacing * 2 + 3, q.y));
            painter_->drawText( QPointF( 20, q.y), QString::number( y));
        }
    }
}

void anim_curves_view_t::draw_time_bar() const
{
    float time = app().document().composition().frame();
    Imath::V2i q( world_to_screen( Imath::V2f( time, 0)));

    QPen pen;
    pen.setColor( QColor( 255, 0, 0));
    pen.setWidth( 2);
    painter_->setPen( pen);
    painter_->drawLine( QPoint( q.x, 0), QPoint( q.x, height()));
}

double anim_curves_view_t::nice_num( double x, bool round) const
{
    int expv = std::floor( std::log10( (double) x));
    double f = x / std::pow( 10.0, expv);		/* between 1 and 10 */
    double nf;									/* nice, rounded fraction */

    if (round)
    if (f<1.5) nf = 1.;
	else if (f<3.) nf = 2.;
	else if (f<7.) nf = 5.;
	else nf = 10.;
    else
	if (f<=1.) nf = 1.;
	else if (f<=2.) nf = 2.;
	else if (f<=5.) nf = 5.;
	else nf = 10.;
    return nf * std::pow(10.0, expv);
}

void anim_curves_view_t::frame_area( const Imath::Box2f& area)
{
	if( area.isEmpty())
		return;
	
    Imath::Box2f new_area( area);
    new_area.min.x -= area.size().x / 10;
    new_area.max.x += area.size().x / 10;
    new_area.min.y -= area.size().y / 10;
    new_area.max.y += area.size().y / 10;

	if( new_area.size().x == 0)
	{
		float half_size = viewport_.world().size().x / 2;
		new_area.min.x -= half_size;
	    new_area.max.x += half_size;
	}

	if( new_area.size().y == 0)
	{
		float half_size = viewport_.world().size().y / 2;
		new_area.min.y -= half_size;
	    new_area.max.y += half_size;
	}

    viewport_.reset( viewport_.device(), new_area);
}

// draw / pick helpers

int anim_curves_view_t::span_num_steps( float t0, float t1) const
{
    float pixel_len = ( t1 - t0) * time_scale();
    return adobe::clamp( (int) pixel_len / 5, 5, 50);
}

void anim_curves_view_t::draw_small_box( const Imath::V2f& p) const
{
    painter()->drawRect( QRectF( p.x - 3, p.y - 3, 5, 5));
}

void anim_curves_view_t::draw_small_box( const Imath::V2i& p) const
{
    painter()->drawRect( QRect( p.x - 3, p.y - 3, 5, 5));
}

float anim_curves_view_t::tangent_length() const { return 20.0f;}

Imath::V2f anim_curves_view_t::left_tangent_dir( float tangent, float yscale) const
{
    Imath::V2f tv( -time_scale(), tangent * value_scale() * yscale);
	tv.normalize();
	return tv;
}

Imath::V2f anim_curves_view_t::right_tangent_dir( float tangent, float yscale) const
{
	Imath::V2f tv( time_scale(), -tangent * value_scale() * yscale);
	tv.normalize();
	return tv;
}

Imath::V2i anim_curves_view_t::left_tangent_pos( const Imath::V2i& p, float tangent, float yscale) const
{
    Imath::V2f tv( left_tangent_dir( tangent, yscale));
	tv *= tangent_length();
    return Imath::V2i( p.x + tv.x, p.y + tv.y);
}

Imath::V2i anim_curves_view_t::right_tangent_pos( const Imath::V2i& p, float tangent, float yscale) const
{
    Imath::V2f tv( right_tangent_dir( tangent, yscale));
	tv *= tangent_length();
    return Imath::V2i( p.x + tv.x, p.y + tv.y);
}

int anim_curves_view_t::pick_distance() const	{ return 5;}
int anim_curves_view_t::pick_distance2() const	{ return pick_distance() * pick_distance();}

bool anim_curves_view_t::inside_pick_distance( const Imath::V2i& p, const Imath::V2i& q) const
{
	int l = ( p - q).length2();
	int m = pick_distance2();
	return l < m;
}

// slots
void anim_curves_view_t::frame_all()
{
    bbox_curve_visitor v;

    BOOST_FOREACH( const anim::track_t *t, app().ui()->anim_editor().active_tracks())
        boost::apply_visitor( v, t->curve().get());

	frame_area( v.bbox);
}

void anim_curves_view_t::frame_selection()
{
    bbox_curve_visitor v( Imath::Box2f(), true);

    BOOST_FOREACH( const anim::track_t *t, app().ui()->anim_editor().active_tracks())
        boost::apply_visitor( v, t->curve().get());

	frame_area( v.bbox);
}

void anim_curves_view_t::set_snap_frames( bool b)	{ snap_ = b;}

void anim_curves_view_t::set_show_tangents( bool b)
{ 
	show_tangents_ = b;
	update();
}

} // namespace
} // namespace
