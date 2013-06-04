// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/widgets/double_spinbox.hpp>

#include<QContextMenuEvent>
#include<QMenu>

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

double_spinbox_t::double_spinbox_t( QWidget *parent) : spinbox_t( parent)
{
	value_ = 0;
	previous_value_ = 0;
	setText( "0");
    connect( this, SIGNAL( editingFinished()), this, SLOT( textChanged()));
}

void double_spinbox_t::setSuffix( const QString& s)
{
	// TODO: implement this
}

double double_spinbox_t::value() const { return value_;}

bool double_spinbox_t::setValue( double v)
{
	double new_val = v;
	
	if( decimals() == 0)
		new_val = (int) new_val;

    new_val = clamp( new_val, minimum(), maximum());
	spinbox_t::setLineEditContents( new_val);
	
    new_val = clamp( new_val, minimum(), maximum());
	
	if( new_val == value_)
		return false;

	previous_value_ = new_val;
	value_ = new_val;
	return true;
}

void double_spinbox_t::restorePreviousValue() { setValue( previous_value_);}

void double_spinbox_t::keyPressEvent( QKeyEvent *event)
{
	if( event->key() == Qt::Key_Up || event->key() == Qt::Key_Down)
	{
		if( !event->isAutoRepeat())
		{
			if( trackMouse())
				spinBoxPressed();
		}
		
		stepBy( event->key() == Qt::Key_Up ? 1 : -1);
		return;
	}
	else
		QLineEdit::keyPressEvent( event);
}

void double_spinbox_t::keyReleaseEvent( QKeyEvent *event)
{
	if( event->key() == Qt::Key_Up || event->key() == Qt::Key_Down)
	{
		if( trackMouse())
			spinBoxReleased();
		else
			valueChanged( value_);		
	}
	else
		QLineEdit::keyReleaseEvent( event);
}

void double_spinbox_t::mousePressEvent( QMouseEvent *event)
{
	drag_ = false;
	dragged_ = false;
	first_drag_ = true;
	
	if(( event->button() == Qt::LeftButton) && ( event->modifiers() & Qt::AltModifier))
	{
		drag_ = true;
		push_x_ = event->x();
		last_x_ = push_x_;
	}
	else
		QLineEdit::mousePressEvent( event);
}

void double_spinbox_t::mouseMoveEvent( QMouseEvent *event)
{
	if( drag_)
	{
		if( event->x() != last_x_)
		{
			if( first_drag_)
			{
				// add some tolerance for wacom tablets
				if( abs( event->x() - push_x_) >= 3)
				{
					if( trackMouse())
						spinBoxPressed();
				
					first_drag_ = false;
					dragged_ = true;
				}
				else
					return;
			}
			
			stepBy( ( event->x() - last_x_));
		}
		
		last_x_ = event->x();
	}
	else
		QLineEdit::mouseMoveEvent( event);
}

void double_spinbox_t::mouseReleaseEvent( QMouseEvent *event)
{
	if( drag_)
	{
		if( dragged_)
		{
			if( trackMouse())
				spinBoxReleased();
			else
				valueChanged( value_);
		}
		
		dragged_ = false;
		drag_ = false;
		first_drag_ = true;
	}
	else
		QLineEdit::mouseReleaseEvent( event);	
}

void double_spinbox_t::contextMenuEvent( QContextMenuEvent *event)
{
	/*
	QMenu menu( this);
	menu.addAction( copy_);
	menu.addSeparator();
	menu.addAction( paste_);
	menu.exec(event->globalPos());
	event->accept();
	*/
}

void double_spinbox_t::textChanged()
{
	if( isModified())
	{
		setModified( false);
		std::string s( text().toStdString());
		boost::optional<double> result = calc()( s);
		
		if( result)
		{
			if( setValue( result.get())) // it's a simple formula.
				valueChanged( value());
		}
		else
			restorePreviousValue();
	}
}

void double_spinbox_t::stepBy( int steps)
{
	if( setValue( value() + steps * singleStep()))
	{
		if( trackMouse())
			spinBoxDragged( value());
	}
}

} // ui
} // ramen
