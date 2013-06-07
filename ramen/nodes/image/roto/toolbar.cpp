// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/roto/toolbar.hpp>

#include<QHBoxLayout>
#include<QSpacerItem>
#include<QButtonGroup>
#include<QToolButton>

#include<ramen/nodes/image/roto/roto_node.hpp>

#include<ramen/nodes/image/roto/convert_tool.hpp>
#include<ramen/nodes/image/roto/create_tool.hpp>
#include<ramen/nodes/image/roto/rect_tool.hpp>
#include<ramen/nodes/image/roto/select_tool.hpp>
#include<ramen/nodes/image/roto/null_tool.hpp>

#include<ramen/app/application.hpp>
#include<ramen/ui/user_interface.hpp>

namespace ramen
{
namespace roto
{

toolbar_t::toolbar_t( image::roto_node_t& parent) : parent_( parent), QWidget()
{
    setMinimumSize( 0, app().ui()->viewer_toolbar_height());
    setMaximumSize( QWIDGETSIZE_MAX, app().ui()->viewer_toolbar_height());

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins( 0, 0, 0, 0);

    QToolButton *select_ = new QToolButton( this);
    select_->setIcon( QIcon( ":icons/arrow.png"));
    select_->setFocusPolicy( Qt::NoFocus);
    select_->setCheckable(true);
    select_->setChecked(true);
    select_->setToolTip( "Select / drag curves and points");
    connect( select_, SIGNAL( toggled( bool)), this, SLOT( pick_select_tool( bool)));
    layout->addWidget( select_);

    QToolButton *create_ = new QToolButton( this);
    create_->setIcon( QIcon( ":icons/pen.png"));
    create_->setFocusPolicy( Qt::NoFocus);
    create_->setCheckable(true);
    create_->setChecked(false);
    create_->setToolTip( "Create curves, add points to existing curves");
    connect( create_, SIGNAL( toggled( bool)), this, SLOT( pick_create_tool( bool)));
    layout->addWidget( create_);

    QToolButton *box_ = new QToolButton( this);
    box_->setIcon( QIcon( ":icons/box.png"));
    box_->setFocusPolicy( Qt::NoFocus);
    box_->setCheckable(true);
    box_->setChecked(false);
    box_->setToolTip( "Create rectangles");
    connect( box_, SIGNAL( toggled( bool)), this, SLOT( pick_rect_tool( bool)));
    layout->addWidget( box_);

    QToolButton *null_ = new QToolButton( this);
    null_->setIcon( QIcon( ":icons/null.png"));
    null_->setFocusPolicy( Qt::NoFocus);
    null_->setCheckable(true);
    null_->setChecked(false);
    null_->setToolTip( "Create nulls");
    connect( null_, SIGNAL( toggled( bool)), this, SLOT( pick_null_tool( bool)));
    layout->addWidget( null_);
	
    QToolButton *convert_ = new QToolButton( this);
    convert_->setIcon( QIcon( ":icons/linear_tan.png"));
    convert_->setFocusPolicy( Qt::NoFocus);
    convert_->setCheckable(true);
    convert_->setChecked(false);
    convert_->setToolTip( "Convert points to corner/curve points");
    connect( convert_, SIGNAL( toggled( bool)), this, SLOT( pick_convert_tool( bool)));
    layout->addWidget( convert_);

    QSpacerItem *s = new QSpacerItem( 30, 30, QSizePolicy::Expanding, QSizePolicy::Minimum);
    layout->addItem( s);

    setLayout( layout);

    QButtonGroup *bgroup_ = new QButtonGroup( this);
    bgroup_->addButton( create_);
    bgroup_->addButton( select_);
    bgroup_->addButton( convert_);
    bgroup_->addButton( box_);
	bgroup_->addButton( null_);

	select_tool_ = new select_tool_t( parent);
	create_tool_ = new create_tool_t( parent);
	rect_tool_ = new rect_tool_t( parent);
	convert_tool_ = new convert_tool_t( parent);
	null_tool_ = new null_tool_t( parent);

	current_tool_ = select_tool_;
	current_tool_->begin_active();
}

toolbar_t::~toolbar_t()
{
	delete select_tool_;
	delete create_tool_;
	delete rect_tool_;
	delete convert_tool_;
}

const tool_t& toolbar_t::active_tool() const	{ return *current_tool_;}
tool_t& toolbar_t::active_tool()				{ return *current_tool_;}

void toolbar_t::pick_select_tool( bool b)
{
	if( b)
	{
		current_tool_->end_active();
		current_tool_ = select_tool_;
		current_tool_->begin_active();
	}
}

void toolbar_t::pick_create_tool( bool b)
{
	if( b)
	{
		current_tool_->end_active();
		current_tool_ = create_tool_;
		current_tool_->begin_active();
	}
}

void toolbar_t::pick_rect_tool( bool b)
{
	if( b)
	{
		current_tool_->end_active();
		current_tool_ = rect_tool_;
		current_tool_->begin_active();
	}
}

void toolbar_t::pick_convert_tool( bool b)
{
	if( b)
	{
		current_tool_->end_active();
		current_tool_ = convert_tool_;
		current_tool_->begin_active();
	}
}

void toolbar_t::pick_null_tool( bool b)
{
	if( b)
	{
		current_tool_->end_active();
		current_tool_ = null_tool_;
		current_tool_->begin_active();
	}
}

} // namespace
} // namespace
