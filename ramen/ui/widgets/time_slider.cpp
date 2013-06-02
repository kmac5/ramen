// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/widgets/time_slider.hpp>

#include<algorithm>
#include<cmath>

#include<QHBoxLayout>
#include<QSlider>
#include<QPainter>

#include<QMouseEvent>
#include<QPaintEvent>

#include<ramen/ui/widgets/double_spinbox.hpp>

#include<ramen/ui/nice_numbers.hpp>

namespace ramen
{
namespace ui
{
namespace
{

template<class T>
T clamp( T x, T lo, T hi)
{
    if( x < lo)
        return lo;

    if( x > hi)
        return hi;

    return x;
}

} // unnamed

time_scale_t::time_scale_t( QWidget *parent) : QWidget( parent)
{
    min_value_ = 1;
    max_value_ = 100;
    value_ = 1;
	drag_ = false;
}

void time_scale_t::setRange( int lo, int hi)
{
    min_value_ = lo;
    max_value_ = hi;
    update();
}

void time_scale_t::setMinimum( int m)
{
    min_value_ = m;
    update();
}

void time_scale_t::setMaximum( int m)
{
    max_value_ = m;
    update();
}

void time_scale_t::setValue( int v)
{
    int new_val = clamp( v, min_value_, max_value_);

    if( value_ != new_val)
    {
		value_ = new_val;
		valueChanged( v);
		update();
    }
}

int time_scale_t::round_halfup( float x) const
{
	int result = std::floor( std::fabs( x) + 0.5);
	return ( x < 0.0) ? -result : result;
}

int time_scale_t::frame_from_mouse_pos( int x) const
{
	float f = ( float) x / width();
	f = f * ( max_value_ - min_value_) + min_value_;
    return clamp( round_halfup( f), min_value_, max_value_);
}

void time_scale_t::mousePressEvent( QMouseEvent *event)
{
	if( event->button() == Qt::LeftButton)
	{
		drag_ = true;
	    last_x_ = event->x();
	    setValue( frame_from_mouse_pos( last_x_));
	}

	event->accept();
}

void time_scale_t::mouseMoveEvent( QMouseEvent *event)
{
	if( drag_)
	{
	    if( last_x_ != event->x())
			setValue( frame_from_mouse_pos( event->x()));			

	    last_x_ = event->x();
	}

	event->accept();
}

void time_scale_t::mouseReleaseEvent( QMouseEvent *event) { event->accept();}

void time_scale_t::paintEvent ( QPaintEvent *event)
{
    QPainter painter( this);
    painter.setRenderHint( QPainter::Antialiasing);

    QPen pen;
    pen.setColor( QColor( 0, 0, 0));
    pen.setWidth( 1);
    painter.setPen( pen);

    painter.drawLine( 0, 7, width(), 7);

    const int spacing = 50;
    int nticks = std::floor( (double) width() / spacing);
    nice_numbers_t numbers( min_value_, max_value_, nticks);

    for( nice_numbers_t::iterator it( numbers.begin()); it < numbers.end(); ++it)
    {
		float x = *it;
		float sx = ( x - min_value_) / ( max_value_ - min_value_) * width();
		painter.drawLine( QPointF( sx, 2), QPointF( sx, 12));
        painter.drawText( QPoint( sx, height()), QString::number( x));
    }

    pen.setColor( QColor( 255, 0, 0));
    pen.setWidth( 3);
    painter.setPen( pen);

    float x = (float) (value_ - min_value_) / (float) ( max_value_ - min_value_) * width();
    painter.drawLine( QPointF( x, 0), QPointF( x, height()));

    event->accept();
}

time_slider_t::time_slider_t( QWidget *parent) : QWidget( parent)
{

    start_ = new double_spinbox_t();
	start_->setTrackMouse( false);
    start_->setRange( -32768, 32768);
    start_->setValue( 1);
    start_->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);

    end_ = new double_spinbox_t();
	end_->setTrackMouse( false);
    end_->setRange( -32768, 32768);
    end_->setValue( 100);
    end_->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);

    current_ = new double_spinbox_t();
	current_->setTrackMouse( false);
    current_->setRange(1, 100);
    current_->setValue( 1);
    current_->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);

    scale_ = new time_scale_t();
    scale_->setRange(1, 100);
    scale_->setValue( 1);

    connect( start_		, SIGNAL( valueChanged(double)), this, SLOT( set_start_frame(double)));
    connect( end_		, SIGNAL( valueChanged(double)), this, SLOT( set_end_frame(double)));
    connect( current_	, SIGNAL( valueChanged(double)), this, SLOT( set_frame(double)));
    connect( scale_		, SIGNAL( valueChanged(double)), this, SLOT( set_frame(double)));

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget( start_);
    layout->addWidget( current_);
    layout->addWidget( scale_);
    layout->addWidget( end_);
    setLayout(layout);

    setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed);
}

void time_slider_t::update( int start, int frame, int end)
{
    block_all_signals( true);

    start_->setValue( start);
    end_->setValue( end);

    current_->setMinimum( start);
    current_->setMaximum( end);
    current_->setValue( frame);

    scale_->setMinimum( start);
    scale_->setMaximum( end);
    scale_->setValue( frame);

    block_all_signals( false);
}

void time_slider_t::set_start_frame( double t)
{
    block_all_signals( true);

	int cur_frame = current_->value();
    int new_start = std::min( t, end_->value());
    start_->setValue( new_start);

    current_->setMinimum( start_->value());
    scale_->setMinimum( start_->value());

    block_all_signals( false);

    start_frame_changed( start_->value());
    adjust_frame( cur_frame);
}

void time_slider_t::set_end_frame( double t)
{
    block_all_signals( true);

	int cur_frame = current_->value();
    int new_end = std::max( t, start_->value());
    end_->setValue( new_end);

    current_->setMaximum( end_->value());
    scale_->setMaximum( end_->value());

    block_all_signals( false);

    end_frame_changed( end_->value());
    adjust_frame( cur_frame);
}

void time_slider_t::set_frame( double t)
{
    block_all_signals( true);
    scale_->setValue( t);
    current_->setValue( t);
    block_all_signals( false);
    time_changed( t);
}

void time_slider_t::block_all_signals( bool b)
{
    start_->blockSignals( b);
    end_->blockSignals( b);
    current_->blockSignals( b);
    scale_->blockSignals( b);
}

void time_slider_t::adjust_frame( int frame)
{
    int new_value = clamp( frame, ( int) start_->value(), ( int) end_->value());

    if( new_value != frame)
    {
		block_all_signals( true);
        current_->setValue( new_value);
		scale_->setValue( new_value);
		block_all_signals( false);
		time_changed( new_value);
    }
}

} // namespace
} // namespace
