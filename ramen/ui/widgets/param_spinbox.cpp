// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/python/python.hpp>

#include<ramen/ui/widgets/param_spinbox.hpp>

#include<QAction>
#include<QContextMenuEvent>
#include<QMenu>

#include<ramen/params/animated_param.hpp>

#include<ramen/app/application.hpp>
#include<ramen/app/document.hpp>

#include<ramen/anim/clipboard.hpp>

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

param_spinbox_t::param_spinbox_t( param_t& param, int comp_index, QWidget *parent) : spinbox_t( parent), param_( param)
{
	aparam_ = dynamic_cast<animated_param_t*>( &param_);
	
	comp_index_ = comp_index;
	setText( "0");
	value_ = 0;
	previous_value_ = 0;
    connect( this, SIGNAL( editingFinished()), this, SLOT( textChanged()));
	
	set_key_ = new QAction( "Set key", this);
	connect( set_key_, SIGNAL( triggered()), this, SLOT( set_key()));

	del_key_ = new QAction( "Delete key", this);
	connect( del_key_, SIGNAL( triggered()), this, SLOT( delete_key()));

	del_anim_ = new QAction( "Delete anim", this);
	connect( del_anim_, SIGNAL( triggered()), this, SLOT( delete_anim()));

	copy_anim_ = new QAction( "Copy anim", this);
	connect( copy_anim_, SIGNAL( triggered()), this, SLOT( copy_anim()));

	paste_anim_ = new QAction( "Paste anim", this);
	connect( paste_anim_, SIGNAL( triggered()), this, SLOT( paste_anim()));
	
	del_expr_ = new QAction( "Delete expression", this);
	expr_editor_ = new QAction( "Expression editor...", this);
	copy_ref_ = new QAction( "Copy reference", this);
	paste_ref_ = new QAction( "Paste reference", this);
}

double param_spinbox_t::value() const
{
	RAMEN_ASSERT( value_.which() == 0);
	return boost::get<double>( value_);
}

std::string param_spinbox_t::stringValue() const
{
	RAMEN_ASSERT( value_.which() == 0);
	return boost::get<std::string>( value_);
}

bool param_spinbox_t::setValue( double v)
{
    double new_val = clamp( v, minimum(), maximum());
	spinbox_t::setLineEditContents( new_val);
	
	if( value_.which() == 0)
	{
		if( new_val == boost::get<double>( value_))
			return false;
	}
	
	previous_value_ = new_val;
	value_ = new_val;
	return true;
}

void param_spinbox_t::setValue( const std::string& s)
{
	previous_value_ = s;
	value_ = s;
	setLineEditContents( s);
}

void param_spinbox_t::restorePreviousValue()
{
	if( previous_value_.which() == 0)
		setValue( boost::get<double>( previous_value_));
	else
		setValue( boost::get<std::string>( previous_value_));
}

void param_spinbox_t::keyPressEvent( QKeyEvent *event)
{
	if( value_.which() == 0)
	{
		if( event->key() == Qt::Key_Up || event->key() == Qt::Key_Down)
		{
			if( !event->isAutoRepeat())
				spinBoxPressed();
			
			stepBy( event->key() == Qt::Key_Up ? 1 : -1);
			return;
		}
	}

	QLineEdit::keyPressEvent( event);
}

void param_spinbox_t::keyReleaseEvent( QKeyEvent *event)
{
	if( value_.which() == 0 && event->key() == Qt::Key_Up || event->key() == Qt::Key_Down)
		spinBoxReleased();
	else
		QLineEdit::keyReleaseEvent( event);
}

void param_spinbox_t::mousePressEvent( QMouseEvent *event)
{
	drag_ = false;
	dragged_ = false;
	first_drag_ = true;
	
	if( value_.which() == 0)
	{
		if(( event->button() == Qt::LeftButton) && ( event->modifiers() & Qt::AltModifier))
		{
			drag_ = true;
			push_x_ = event->x();
			last_x_ = push_x_;
			event->accept();
			return;
		}
	}

	QLineEdit::mousePressEvent( event);
}

void param_spinbox_t::mouseMoveEvent( QMouseEvent *event)
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

void param_spinbox_t::mouseReleaseEvent( QMouseEvent *event)
{
	if( drag_)
	{
		if( dragged_)
		{
			if( trackMouse())
				spinBoxReleased();
			else
				valueChanged( value());
		}
		
		dragged_ = false;
		drag_ = false;
		first_drag_ = true;	
	}
	else
		QLineEdit::mouseReleaseEvent( event);	
}

void param_spinbox_t::contextMenuEvent( QContextMenuEvent *event)
{
	QMenu menu( this);

	if( !param_.is_static())
	{
		bool has_anim = false;
		bool has_key = false;
		
		if( aparam_)
			has_anim = !aparam_->curve( comp_index_).empty();
		
		if( has_anim)
		{
			float frame = app().document().composition().frame();
			has_key = aparam_->curve( comp_index_).has_keyframe_at( frame);
		}
		
		// enable or disable items
		set_key_->setEnabled( true);
		del_key_->setEnabled( has_key);
		
		copy_anim_->setEnabled( has_anim);
		del_anim_->setEnabled( has_anim);
		
		if( !anim::clipboard_t::instance().can_paste())
			paste_anim_->setEnabled( false);
				
		menu.addAction( set_key_);
		menu.addAction( del_key_);
		menu.addAction( copy_anim_);
		menu.addAction( paste_anim_);
		menu.addAction( del_anim_);
		
		if( param_.can_have_expressions())
			menu.addSeparator();
	}
	
	if( param_.can_have_expressions())
	{
		// enable or disable items
		expr_editor_->setEnabled( false);
		copy_ref_->setEnabled( false);
		paste_ref_->setEnabled( false);
		del_expr_->setEnabled( false);
		
		menu.addAction( expr_editor_);
		menu.addAction( copy_ref_);
		menu.addAction( paste_ref_);
		menu.addAction( del_expr_);
	}

	if( !menu.isEmpty())
	{
		menu.exec(event->globalPos());
		event->accept();
	}
	else
		event->ignore();
}

void param_spinbox_t::textChanged()
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
		{
			if( param_.can_have_expressions())
			{
				setValue( s); // in this case, it's a potential expression
				expressionSet();
			}
			else
				restorePreviousValue();
		}
	}
}

void param_spinbox_t::setLineEditContents( const std::string& s)
{
	setText( QString::fromStdString( s));
	setModified( false);
}

void param_spinbox_t::stepBy( int steps)
{
	if( setValue( value() + steps * singleStep()))
	{
		if( trackMouse())
			spinBoxDragged( value());
	}
}

// slots
void param_spinbox_t::set_key()
{
	RAMEN_ASSERT( aparam_);
	
	aparam_->set_key( comp_index_);
}

void param_spinbox_t::copy_anim()
{
	RAMEN_ASSERT( aparam_);

	anim::clipboard_t::instance().begin_copy();
	anim::clipboard_t::instance().copy( aparam_->curve( comp_index_));
	anim::clipboard_t::instance().end_copy();
}

void param_spinbox_t::paste_anim()
{
	RAMEN_ASSERT( aparam_);

	aparam_->paste( comp_index_);
}

void param_spinbox_t::delete_key()
{
	RAMEN_ASSERT( aparam_);

	aparam_->delete_key( comp_index_, app().document().composition().frame());
}

void param_spinbox_t::delete_anim()
{
	RAMEN_ASSERT( aparam_);

	aparam_->delete_all_keys( comp_index_);
}

} // ui
} // ramen
