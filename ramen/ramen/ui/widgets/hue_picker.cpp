// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/widgets/hue_picker.hpp>

#include<QPaintEvent>
#include<QMouseEvent>
#include<QResizeEvent>
#include<QPainter>

namespace ramen
{
namespace ui
{

hue_picker_t::hue_picker_t( QWidget *parent) : QWidget( parent), background_( sizeHint(), QImage::Format_RGB32)
{
    setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding);
    setFocusPolicy( Qt::StrongFocus);
    valid_background_ = false;
    hue_ = 0;
}

QSize hue_picker_t::sizeHint() const { return QSize( 15, 200);}

double hue_picker_t::hue() const { return hue_;}

void hue_picker_t::set_hue( double h)
{
    if( hue_ != h)
    {
        hue_ = h;
        hue_changed( hue_);
        update();
    }
}

void hue_picker_t::paintEvent( QPaintEvent *event)
{
    QPainter painter( this);

    if( event->rect().intersects(contentsRect()))
        painter.setClipRegion( event->region().intersect(contentsRect()));
    
    if( !valid_background_)
    {
        update_background();
        valid_background_ = true;
    }

    painter.drawImage( contentsRect().topLeft(), background_);

    int ypos = ( 1.0f - hue_) * height();

    QPen pen;
    pen.setWidth( 1);

    pen.setColor( QColor( 255, 255, 255));
    painter.setPen( pen);
    painter.drawLine( 0, ypos, width(), ypos);

    ++ypos;
    pen.setColor( QColor( 0, 0, 0));
    painter.setPen( pen);
    painter.drawLine( 0, ypos, width(), ypos);
}

void hue_picker_t::mousePressEvent( QMouseEvent *event)
{
    set_hue( pick_hue( event->y()));
    event->accept();
}

void hue_picker_t::mouseMoveEvent( QMouseEvent *event)
{
    set_hue( pick_hue( event->y()));
    event->accept();
}

void hue_picker_t::mouseReleaseEvent( QMouseEvent *event) { event->accept();}

void hue_picker_t::resizeEvent( QResizeEvent *event)
{
    valid_background_ = false;
    update();
}

double hue_picker_t::pick_hue( int y) const
{
    double h = 1.0f - ((double) y / ( height() - 1));
    if( h < 0)  h = 0;
    if( h >= 1) h = 0;
    return h;
}

void hue_picker_t::update_background()
{
    background_ = QImage(contentsRect().size(), QImage::Format_RGB32);

    QColor col;

    for( int j = 0; j < background_.height(); ++j)
    {
        QRgb *p = reinterpret_cast<QRgb*>( background_.scanLine( j));

        double h = 1.0f - ((double) j / ( background_.height() - 1));
        if( h < 0) h = 0;
        if( h > 1) h = 0;

        col.setHsvF( h, 1.0f, 1.0f);
        QRgb q = col.rgb();

        for( int i = 0; i < background_.width(); ++i)
            *p++ = q;
    }
}

} // ui
} // ramen
