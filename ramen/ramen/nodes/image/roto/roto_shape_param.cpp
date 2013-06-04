// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/roto/roto_shape_param.hpp>

#include<boost/foreach.hpp>

#include<QComboBox>
#include<QLabel>
#include<QToolButton>
#include<QColorDialog>
#include<QPushButton>

#include<ramen/assert.hpp>

#include<ramen/app/application.hpp>
#include<ramen/app/document.hpp>

#include<ramen/undo/stack.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/inspector/inspector.hpp>
#include<ramen/ui/viewer/viewer.hpp>
#include<ramen/ui/anim/anim_editor.hpp>

#include<ramen/ui/widgets/line_edit.hpp>
#include<ramen/ui/widgets/color_button.hpp>

#include<ramen/nodes/image/roto/roto_node.hpp>
#include<ramen/nodes/image/roto/shape.hpp>
#include<ramen/nodes/image/roto/roto_commands.hpp>

namespace ramen
{

roto_shape_param_t::roto_shape_param_t( const std::string& id) : static_param_t( "Shape"), selected_( 0)
{
	set_id( id);
}

roto_shape_param_t::roto_shape_param_t( const roto_shape_param_t& other) : static_param_t( other)
{
	name_input_ = 0;
	parent_menu_ = 0;
	display_color_ = 0;
	autokey_ = 0;
	shape_key_ = 0;
	selected_ = 0;
	order_up_ = 0;
	order_down_ = 0;
}

param_t *roto_shape_param_t::do_clone() const { return new roto_shape_param_t( *this);}

QWidget *roto_shape_param_t::do_create_widgets()
{
    QWidget *top = new QWidget();
    QLabel *label = new QLabel( top);
    name_input_ = new ui::line_edit_t( top);

	QSize s = name_input_->sizeHint();

    label->move( 0, 0);
    label->resize( app().ui()->inspector().left_margin() - 5, s.height());
    label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
    label->setText( "Name");

    name_input_->move( app().ui()->inspector().left_margin(), 0);
    name_input_->resize( app().ui()->inspector().width() - app().ui()->inspector().left_margin() - 80, s.height());
	name_input_->setEnabled( false);
    connect( name_input_, SIGNAL( editingFinished()), this, SLOT( rename_shape()));
    int current_height = s.height() + 5;
	
    label = new QLabel( top);
    label->move( 0, current_height);
    label->resize( app().ui()->inspector().left_margin() - 5, s.height());
    label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
    label->setText( "Parent");

    parent_menu_ = new QComboBox( top);
    s = parent_menu_->sizeHint();
    parent_menu_->setFocusPolicy( Qt::NoFocus);
	parent_menu_->addItem( "None");
    s = parent_menu_->sizeHint();
    parent_menu_->move( app().ui()->inspector().left_margin(), current_height);
	parent_menu_->setEnabled( false);
    connect( parent_menu_, SIGNAL( currentIndexChanged( int)), this, SLOT( set_shape_parent( int)));
	
	int w = app().ui()->inspector().left_margin() + s.width() + 5;
    order_up_ = new QToolButton( top);
    order_up_->setFocusPolicy( Qt::NoFocus);
    order_up_->setArrowType( Qt::UpArrow);
    order_up_->setToolTip( "Move shape order up.");
    order_up_->setEnabled( false);
	order_up_->move( w, current_height);
	order_up_->resize( s.height(), s.height());
    connect( order_up_, SIGNAL( pressed()), this, SLOT( move_shape_order_up()));
	w += s.height() + 5;
	
    order_down_ = new QToolButton( top);
    order_down_->setFocusPolicy( Qt::NoFocus);
    order_down_->setArrowType( Qt::DownArrow);
    order_down_->setToolTip( "Move shape order down.");
    order_down_->setEnabled( false);
	order_down_->move( w, current_height);
	order_down_->resize( s.height(), s.height());
    connect( order_down_, SIGNAL( pressed()), this, SLOT( move_shape_order_down()));
	current_height += s.height() + 5;
	
    label = new QLabel( top);
    label->move( 0, current_height);
    label->resize( app().ui()->inspector().left_margin() - 5, s.height());
    label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
    label->setText( "Display Color");

	display_color_ = new ui::color_button_t( top);
	display_color_->move( app().ui()->inspector().left_margin(), current_height);
	display_color_->resize( s.height(), s.height());
	display_color_->setEnabled( false);
    connect( display_color_, SIGNAL( pressed()), this, SLOT( change_shape_color()));
	
	autokey_ = new QToolButton( top);
	autokey_->setText( "Autokey");
	autokey_->setCheckable( true);
	w = app().ui()->inspector().left_margin() + s.height() + 5;
	autokey_->move( w, current_height);
	autokey_->setEnabled( false);
	connect( autokey_, SIGNAL( toggled( bool)), this, SLOT( set_autokey(bool)));
	s = autokey_->sizeHint();
	
	shape_key_ = new QPushButton( top);
	shape_key_->setText( "Set Shape Key");
	w = w + s.width() + 5;
	shape_key_->move( w, current_height);
	connect( shape_key_, SIGNAL( pressed()), this, SLOT( set_shape_key()));
	s = shape_key_->sizeHint();
	current_height += s.height() + 5;
	
    top->setMinimumSize( app().ui()->inspector().width(), current_height);
    top->setMaximumSize( app().ui()->inspector().width(), current_height);
    top->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);
	
	update_parent_menu();
	set_active_shape( selected_);
    return top;
}

void roto_shape_param_t::set_active_shape( roto::shape_t *s)
{
	selected_ = s;
	
	if( !name_input_)
		return;

	name_input_->blockSignals( true);
	parent_menu_->blockSignals( true);
	display_color_->blockSignals( true);
	autokey_->blockSignals( true);
	
	if( s)
	{
		name_input_->setEnabled( true);
		name_input_->setText( s->name().c_str());

		// update parent menu here
		if( s->parent() == 0)
			parent_menu_->setCurrentIndex( 0);
		else
		{
			QString pname = s->parent_name().c_str();
			bool parent_found = false;
			
			for( int i = 0; i < parent_menu_->count(); ++i)
			{
				if( parent_menu_->itemText( i) == pname)
				{
					parent_menu_->setCurrentIndex( i);
					parent_found = true;
					break;
				}
			}
			
			RAMEN_ASSERT( parent_found);
		}

		parent_menu_->setEnabled( true);

		ui::color_t c( s->display_color().x / 255.0f,
				   s->display_color().y / 255.0f, 
				   s->display_color().z / 255.0f);
		display_color_->set_value( c);
		display_color_->setEnabled( true);
		
		autokey_->setEnabled( true);
		autokey_->setChecked( selected_->autokey());
		shape_key_->setEnabled( true);
		
		order_down_->setEnabled( !s->is_first_shape());
		order_up_->setEnabled( !s->is_last_shape());
	}
	else
	{
		name_input_->setEnabled( false);
		parent_menu_->setEnabled( false);
		parent_menu_->setCurrentIndex( 0);
		display_color_->setEnabled( false);
		display_color_->set_value( ui::color_t( 0, 0, 0));
		autokey_->setEnabled( false);		
		shape_key_->setEnabled( false);
		order_up_->setEnabled( false);
		order_down_->setEnabled( false);
	}

	name_input_->blockSignals( false);
	parent_menu_->blockSignals( false);
	display_color_->blockSignals( false);
	autokey_->blockSignals( false);
}

void roto_shape_param_t::shape_list_changed()
{
	if( parent_menu_)
	{
		update_parent_menu();
	
		if( selected_)
		{
			order_up_->setEnabled( !selected_->is_first_shape());
			order_down_->setEnabled( !selected_->is_last_shape());
		}
	}
}

void roto_shape_param_t::update_parent_menu()
{
	parent_menu_->blockSignals( true);
	std::string item = parent_menu_->currentText().toStdString();
	
	parent_menu_->clear();
	parent_menu_->addItem( "None");

	const image::roto_node_t *node = dynamic_cast<const image::roto_node_t*>( parameterised());
	RAMEN_ASSERT( node);

	int new_index = 0, i = 1;
	
	BOOST_FOREACH( const roto::shape_t& s, node->scene())
	{
		if( s.name() == item)
			new_index = i;
		
		parent_menu_->addItem( s.name().c_str());
		++i;
	}

	parent_menu_->setCurrentIndex( new_index);
	parent_menu_->blockSignals( false);
}

void roto_shape_param_t::rename_shape()
{
	roto_node().scene().rename_shape( selected_, name_input_->text().toStdString());
    name_input_->blockSignals( true);
    name_input_->setText( selected_->name().c_str());
    name_input_->blockSignals( false);
	app().ui()->anim_editor().recreate_tracks( &roto_node());
}

void roto_shape_param_t::set_autokey( bool b) { selected_->set_autokey( b);}

void roto_shape_param_t::change_shape_color()
{
    Imath::Color3f col = selected_->display_color();

    QColor initial( col.x, col.y, col.z);
    QColor c = QColorDialog::getColor( initial, (QWidget *) app().ui()->main_window());

    if( c.isValid())
    {
		selected_->set_display_color( Imath::Color3f( c.red(), c.green(), c.blue()));
		
		display_color_->blockSignals( true);
		ui::color_t c( selected_->display_color().x / 255.0f,
				   selected_->display_color().y / 255.0f, 
				   selected_->display_color().z / 255.0f);
		display_color_->set_value( c);
		display_color_->blockSignals( false);
        parameterised()->update_overlay();
    }
}

void roto_shape_param_t::set_shape_key()
{
	std::auto_ptr<undo::modify_shape_command_t> cmd( new undo::modify_shape_command_t( roto_node(), selected_));
	selected_->set_shape_key();
	app().document().undo_stack().push_back( cmd);
	app().ui()->update();
}

void roto_shape_param_t::set_shape_parent( int index)
{
	roto::shape_t *new_parent_ = 0;
	
	if( index != 0)
	{
		std::string item = parent_menu_->currentText().toStdString();
		new_parent_ = roto_node().scene().find_shape( item);
		
		if( new_parent_ == selected_->parent()) // Nothing to do here
			return;
		
		if( selected_->find_in_children( new_parent_)) // This is an error. Restore the old state.
		{
			parent_menu_->blockSignals( true);
			
			if( selected_->parent())
			{
				QString pname = selected_->parent_name().c_str();
				
				for( int i = 0; i < parent_menu_->count(); ++i)
				{
					if( parent_menu_->itemText( i) == pname)
					{
						parent_menu_->setCurrentIndex( i);
						break;
					}
				}
			}
			else
				parent_menu_->setCurrentIndex( 0);
			
			parent_menu_->blockSignals( false);
			return;
		}
	}

	std::auto_ptr<undo::set_roto_parent_command_t> cmd;
	cmd.reset( new undo::set_roto_parent_command_t( roto_node(), selected_, new_parent_));
	cmd->redo();
	app().document().undo_stack().push_back( cmd);
	app().ui()->update();
}

void roto_shape_param_t::move_shape_order_up()
{
	std::auto_ptr<undo::order_shape_command_t> cmd;
	cmd.reset( new undo::order_shape_command_t( roto_node(), selected_, true));
	cmd->redo();
	app().document().undo_stack().push_back( cmd);
	app().ui()->update();
}

void roto_shape_param_t::move_shape_order_down()
{
	std::auto_ptr<undo::order_shape_command_t> cmd;
	cmd.reset( new undo::order_shape_command_t( roto_node(), selected_, false));
	cmd->redo();
	app().document().undo_stack().push_back( cmd);
	app().ui()->update();
}

const image::roto_node_t& roto_shape_param_t::roto_node() const
{
	const image::roto_node_t *r = dynamic_cast<const image::roto_node_t*>( parameterised());
	RAMEN_ASSERT( r);
	
	return *r;
}

image::roto_node_t& roto_shape_param_t::roto_node()
{
	image::roto_node_t *r = dynamic_cast<image::roto_node_t*>( parameterised());
	RAMEN_ASSERT( r);
	
	return *r;
}

} // namespace
