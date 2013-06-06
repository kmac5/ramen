// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/anim/anim_editor_toolbar.hpp>

#include<QHBoxLayout>
#include<QPushButton>
#include<QSpacerItem>
#include<QButtonGroup>
#include<QToolButton>
#include<QFrame>
#include<QLabel>
#include<QPushButton>
#include<QMenu>
#include<QAction>

#include<ramen/app/application.hpp>

#include<ramen/anim/keyframe.hpp>
#include<ramen/anim/track.hpp>

#include<ramen/nodes/node_fwd.hpp>

#include<ramen/ui/user_interface.hpp>

#include<ramen/ui/widgets/double_spinbox.hpp>

#include<ramen/ui/anim/anim_editor.hpp>
#include<ramen/ui/anim/anim_curves_view.hpp>
#include<ramen/ui/anim/anim_editor_select_tool.hpp>
#include<ramen/ui/anim/edit_keys_visitors.hpp>

#include<ramen/ui/dialogs/smooth_anim_curves_dialog.hpp>

#include<iostream>

namespace ramen
{
namespace ui
{

anim_editor_toolbar_t::anim_editor_toolbar_t( QWidget* parent) : QWidget( parent)
{
    setMinimumSize( 0, 35);
    setMaximumSize( QWIDGETSIZE_MAX, 35);
    setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins( 5, 5, 5, 5);

    select_ = new QToolButton( this);
    select_->setIcon( QIcon( ":icons/arrow.png"));
    select_->setFocusPolicy( Qt::NoFocus);
    select_->setCheckable( true);
    select_->setChecked( true);
    select_->setToolTip( "Select / Move tool");
    //connect( select_, SIGNAL( toggled( bool)), this, SLOT( pick_select_tool( bool)));
    layout->addWidget( select_);

	/*
	scale_ = new QToolButton( this);
	// TODO: make an icon for this tool
    scale_->setIcon( QIcon( ":icons/arrow.png"));
    scale_->setFocusPolicy( Qt::NoFocus);
    scale_->setCheckable( true);
    scale_->setChecked( false);
    scale_->setToolTip( "Scale tool");
    //connect( scale_, SIGNAL( toggled( bool)), this, SLOT( pick_scale_tool( bool)));
	scale_->setEnabled( false);
    layout->addWidget( scale_);
	*/
	
    QFrame *separator = new QFrame();
    separator->setFrameStyle( QFrame::VLine | QFrame::Raised);
    separator->setLineWidth( 1);
    layout->addWidget( separator);

    // tangents
    smooth_tangents_ = new QToolButton( this);
    smooth_tangents_->setIcon( QIcon( ":icons/smooth_tan.png"));
    smooth_tangents_->setFocusPolicy( Qt::NoFocus);
    smooth_tangents_->setToolTip( "Smooth tangents");
    connect( smooth_tangents_, SIGNAL( pressed()), this, SLOT( set_tangents_smooth()));
    layout->addWidget( smooth_tangents_);

    flat_tangents_ = new QToolButton( this);
    flat_tangents_->setIcon( QIcon( ":icons/flat_tan.png"));
    flat_tangents_->setFocusPolicy( Qt::NoFocus);
    flat_tangents_->setToolTip( "Flat tangents");
    connect( flat_tangents_, SIGNAL( pressed()), this, SLOT( set_tangents_flat()));
    layout->addWidget( flat_tangents_);
    
    linear_tangents_ = new QToolButton( this);
    linear_tangents_->setIcon( QIcon( ":icons/linear_tan.png"));
    linear_tangents_->setFocusPolicy( Qt::NoFocus);
    linear_tangents_->setToolTip( "Linear tangents");
    connect( linear_tangents_, SIGNAL( pressed()), this, SLOT( set_tangents_linear()));
    layout->addWidget( linear_tangents_);

    step_tangents_ = new QToolButton( this);
    step_tangents_->setIcon( QIcon( ":icons/step_tan.png"));
    step_tangents_->setFocusPolicy( Qt::NoFocus);
    step_tangents_->setToolTip( "Step interpolation");
    connect( step_tangents_, SIGNAL( pressed()), this, SLOT( set_tangents_step()));
    layout->addWidget( step_tangents_);

    separator = new QFrame();
    separator->setFrameStyle( QFrame::VLine | QFrame::Raised);
    separator->setLineWidth( 1);
    layout->addWidget( separator);

	QLabel *label = new QLabel( "Time");
	layout->addWidget( label);
	
	time_ = new double_spinbox_t();
	time_->setEnabled( false);
	QSize size = time_->sizeHint();	
	time_->setMaximumSize( size);
	time_->resize( time_->sizeHint());
    connect( time_, SIGNAL( valueChanged( double)), this, SLOT( value_changed( double)));
    connect( time_, SIGNAL( spinBoxPressed()), this, SLOT( spinbox_pressed()));
    connect( time_, SIGNAL( spinBoxDragged( double)), this, SLOT( spinbox_dragged( double)));
    connect( time_, SIGNAL( spinBoxReleased()), this, SLOT( spinbox_released()));
	layout->addWidget( time_);

	label = new QLabel( "Value");
	layout->addWidget( label);
	
	val_ = new double_spinbox_t();
	val_->setMaximumSize( size);
	val_->setEnabled( false);
    connect( val_, SIGNAL( valueChanged( double)), this, SLOT( value_changed( double)));
    connect( val_, SIGNAL( spinBoxPressed()), this, SLOT( spinbox_pressed()));
    connect( val_, SIGNAL( spinBoxDragged( double)), this, SLOT( spinbox_dragged( double)));
    connect( val_, SIGNAL( spinBoxReleased()), this, SLOT( spinbox_released()));
	layout->addWidget( val_);

    separator = new QFrame();
    separator->setFrameStyle( QFrame::VLine | QFrame::Raised);
    separator->setLineWidth( 1);
    layout->addWidget( separator);
	
	reverse_ = new QAction( "Reverse", this);
	reverse_->setEnabled( false);
	connect( reverse_, SIGNAL( triggered()), this, SLOT( reverse_selected_keys()));
	
	negate_ = new QAction( "Negate", this);
	negate_->setEnabled( false);
	connect( negate_, SIGNAL( triggered()), this, SLOT( negate_selected_keys()));

	sample_ = new QAction( "Sample", this);
	sample_->setEnabled( false);
	connect( sample_, SIGNAL( triggered()), this, SLOT( sample_selected_keys()));

	smooth_ = new QAction( "Smooth...", this);
	smooth_->setEnabled( false);
	connect( smooth_, SIGNAL( triggered()), this, SLOT( smooth_selected_keys()));

	high_pass_ = new QAction( "High Pass...", this);
	high_pass_->setEnabled( false);
	connect( high_pass_, SIGNAL( triggered()), this, SLOT( high_pass_selected_keys()));
	
	ops_menu_ = new QMenu( this);
	ops_menu_->addAction( reverse_);
	ops_menu_->addAction( negate_);
	ops_menu_->addAction( sample_);
	ops_menu_->addAction( smooth_);
	ops_menu_->addAction( high_pass_);

	ops_button_ = new QPushButton();
	ops_button_->setText( "Operations");
	ops_button_->setMenu( ops_menu_);
	layout->addWidget( ops_button_);
	
    QSpacerItem *s = new QSpacerItem( 30, 30, QSizePolicy::Expanding, QSizePolicy::Minimum);
    layout->addItem( s);
		
    setLayout( layout);

    select_tool_ = new anim_editor_select_tool_t();
	
    bgroup_ = new QButtonGroup( this);
    bgroup_->addButton( select_);
	//bgroup_->addButton( scale_);

	selected_track_ = 0;
	selected_key_index_ = -1;
	drag_command_ = 0;
}

anim_editor_toolbar_t::~anim_editor_toolbar_t() { delete select_tool_;}

anim_editor_tool_t *anim_editor_toolbar_t::tool() { return select_tool_;}

void anim_editor_toolbar_t::selection_changed()
{
	time_->blockSignals( true);
	time_->setEnabled( false);		
	time_->setValue( 0);
	
	val_->blockSignals( true);
	val_->setEnabled( false);		
	val_->setValue( 0);

	reverse_->setEnabled( false);
	negate_->setEnabled( false);
	sample_->setEnabled( false);
	smooth_->setEnabled( false);
	high_pass_->setEnabled( false);
	
	if( app().ui()->anim_editor().any_keyframe_selected())
	{
		reverse_->setEnabled( true);
		
		if( app().ui()->anim_editor().any_float_keyframe_selected())
		{
			negate_->setEnabled( true);
			sample_->setEnabled( true);
			smooth_->setEnabled( true);
			high_pass_->setEnabled( true);
		}
	}
	
	app().ui()->anim_editor().get_selected_keyframe( selected_track_, selected_key_index_);
	
	if( selected_track_)
	{
		RAMEN_ASSERT( selected_key_index_ != -1);
		
		time_->setEnabled( true);
		get_keyframe_time_visitor vt( selected_key_index_);
		boost::apply_visitor( vt, selected_track_->curve().get());
		time_->setValue( vt.time);
		
		get_keyframe_value_visitor vv( selected_key_index_);
		boost::apply_visitor( vv, selected_track_->curve().get());
		val_->setValue( vv.value);
		val_->setEnabled( vv.can_edit);
	}
	
	time_->blockSignals( false);
	val_->blockSignals( false);
}

void anim_editor_toolbar_t::set_tangents_smooth()
{
	app().ui()->anim_editor().set_autotangents( anim::keyframe_t::tangent_smooth, true);
}

void anim_editor_toolbar_t::set_tangents_flat()
{
	app().ui()->anim_editor().set_autotangents( anim::keyframe_t::tangent_flat, true);
}

void anim_editor_toolbar_t::set_tangents_linear()
{
	app().ui()->anim_editor().set_autotangents( anim::keyframe_t::tangent_linear, true);
}

void anim_editor_toolbar_t::set_tangents_step()
{
	app().ui()->anim_editor().set_autotangents( anim::keyframe_t::tangent_step, true);
}

void anim_editor_toolbar_t::value_changed( double value)
{
	spinbox_pressed();
	spinbox_dragged( value);
	spinbox_released();
}

void anim_editor_toolbar_t::spinbox_pressed()
{
	RAMEN_ASSERT( selected_track_);
	RAMEN_ASSERT( selected_key_index_ != -1);

	node_t *n = app().ui()->active_node();
	RAMEN_ASSERT( n);
	
	drag_command_ = new undo::drag_keys_command_t( n, app().ui()->anim_editor().track_model());
	app().ui()->anim_editor().set_command( drag_command_);
}

void anim_editor_toolbar_t::spinbox_dragged( double value)
{
	Imath::V2f offset( 0, 0);
	
	if( sender() == time_)
	{
		get_keyframe_time_visitor v( selected_key_index_);
		boost::apply_visitor( v, selected_track_->curve().get());
		offset.x = value - v.time;
	}
	else
	{
		get_keyframe_value_visitor v( selected_key_index_);
		boost::apply_visitor( v, selected_track_->curve().get());
		offset.y = value - v.value;
	}

	drag_command_->start_drag( offset, false);
	drag_command_->drag_curve( selected_track_);
	drag_command_->end_drag();
	app().ui()->update_anim_editors();
}

void anim_editor_toolbar_t::spinbox_released()
{
	app().ui()->anim_editor().push_command();
	drag_command_ = 0;
	app().ui()->update();
}

void anim_editor_toolbar_t::reverse_selected_keys()
{
	app().ui()->anim_editor().reverse_keyframes();
}
		
void anim_editor_toolbar_t::negate_selected_keys()
{
	app().ui()->anim_editor().negate_keyframes();
}

void anim_editor_toolbar_t::sample_selected_keys()
{
	app().ui()->anim_editor().sample_keyframes();
}

void anim_editor_toolbar_t::smooth_selected_keys()
{
	float filter_size;
	bool resample;
	
	if( smooth_anim_curves_dialog_t::instance().exec( false, filter_size, resample))
		app().ui()->anim_editor().smooth_keyframes( filter_size, resample);
}

void anim_editor_toolbar_t::high_pass_selected_keys()
{
	float filter_size;
	bool resample;
	
	if( smooth_anim_curves_dialog_t::instance().exec( true, filter_size, resample))
		app().ui()->anim_editor().high_pass_keyframes( filter_size, resample);
}

} // namespace
} // namespace
