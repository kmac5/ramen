// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/nodes/image/key/keyer3d/toolbar.hpp>

#include<QHBoxLayout>
#include<QSpacerItem>
#include<QToolButton>
#include<QFrame>
#include<QButtonGroup>

#include<ramen/app/application.hpp>
#include<ramen/ui/user_interface.hpp>

#include<ramen/nodes/image/key/keyer3d/keyer3d_node.hpp>
#include<ramen/nodes/image/key/keyer3d/sample_tool.hpp>
#include<ramen/nodes/image/key/keyer3d/softness_tool.hpp>
#include<ramen/nodes/image/key/keyer3d/tolerance_tool.hpp>

namespace ramen
{
namespace keyer3d
{

toolbar_t::toolbar_t( image::keyer3d_node_t& parent) : parent_( parent), QWidget()
{
    setMinimumSize( 0, app().ui()->viewer_toolbar_height());
    setMaximumSize( QWIDGETSIZE_MAX, app().ui()->viewer_toolbar_height());

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins( 0, 0, 0, 0);

    sample_ = new QToolButton( this);
	sample_->setText( "S");
    sample_->setFocusPolicy( Qt::NoFocus);
    sample_->setCheckable( true);
	sample_->setChecked( true);
    //sample_->setToolTip( "Adjust key tolerance.");
    connect( sample_, SIGNAL( toggled( bool)), this, SLOT( pick_sample_tool( bool)));
    layout->addWidget( sample_);

    tolerance_ = new QToolButton( this);
	tolerance_->setText( "T");
    tolerance_->setFocusPolicy( Qt::NoFocus);
    tolerance_->setCheckable( true);
	tolerance_->setChecked( true);
    tolerance_->setToolTip( "Adjust key tolerance.");
    connect( tolerance_, SIGNAL( toggled( bool)), this, SLOT( pick_tolerance_tool( bool)));
    layout->addWidget( tolerance_);

    softness_ = new QToolButton( this);
	softness_->setText( "S");
    softness_->setFocusPolicy( Qt::NoFocus);
    softness_->setCheckable( true);
    softness_->setToolTip( "Adjust key softness.");
    connect( softness_, SIGNAL( toggled( bool)), this, SLOT( pick_softness_tool( bool)));
    layout->addWidget( softness_);

    QSpacerItem *s = new QSpacerItem( 30, 30, QSizePolicy::Expanding, QSizePolicy::Minimum);
    layout->addItem( s);
    setLayout( layout);

    QButtonGroup *bgroup_ = new QButtonGroup( this);
	bgroup_->addButton( sample_);
    bgroup_->addButton( tolerance_);
    bgroup_->addButton( softness_);

	sample_tool_ = new sample_tool_t( parent);
	tolerance_tool_ = new tolerance_tool_t( parent);
	softness_tool_ = new softness_tool_t( parent);
	current_tool_ = sample_tool_;
	current_tool_->begin_active();
}

toolbar_t::~toolbar_t()
{
	delete sample_tool_;
	delete tolerance_tool_;
	delete softness_tool_;
}

const tool_t *toolbar_t::active_tool() const	{ return current_tool_;}
tool_t *toolbar_t::active_tool()				{ return current_tool_;}

void toolbar_t::pick_sample_tool( bool b)
{
	if( b)
	{
		current_tool_->end_active();
		current_tool_ = sample_tool_;
		current_tool_->begin_active();
	}
}

void toolbar_t::pick_tolerance_tool( bool b)
{	
	if( b)
	{
		current_tool_->end_active();
		current_tool_ = tolerance_tool_;
		current_tool_->begin_active();
	}
}

void toolbar_t::pick_softness_tool( bool b)
{	
	if( b)
	{
		current_tool_->end_active();
		current_tool_ = softness_tool_;
		current_tool_->begin_active();
	}
}

} // namespace
} // namespace
