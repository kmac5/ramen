// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/viewer/image_view/image_view_toolbar.hpp>

#include<QHBoxLayout>
#include<QComboBox>
#include<QToolButton>

#include<ramen/app/application.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/viewer/image_view/image_viewer_strategy.hpp>

namespace ramen
{
namespace ui
{
namespace viewer
{

image_view_toolbar_t::image_view_toolbar_t( image_viewer_strategy_t *image_viewer) : QWidget()
{
    image_viewer_ = image_viewer;

    setMinimumSize( 0, app().ui()->viewer_toolbar_height());
    setMaximumSize( QWIDGETSIZE_MAX, app().ui()->viewer_toolbar_height());

    setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed);
    QHBoxLayout *horizontalLayout = new QHBoxLayout( this);
    horizontalLayout->setContentsMargins( 0, 0, 0, 0);
	
	resolution_combo_ = new QComboBox();
	resolution_combo_->setFocusPolicy( Qt::NoFocus);
	resolution_combo_->insertItems(0, QStringList() << "Full" << "Half" << "Third" << "Quarter");
	resolution_combo_->setMinimumSize( resolution_combo_->sizeHint());
	resolution_combo_->setToolTip( "Preview resolution");
	connect( resolution_combo_, SIGNAL( activated( int)), image_viewer_, SLOT( change_resolution( int)));
	horizontalLayout->addWidget( resolution_combo_);
	
    channels_combo_ = new QComboBox();
	channels_combo_->setFocusPolicy( Qt::NoFocus);
	channels_combo_->insertItems(0, QStringList() << "RGB" << "Red" << "Green" << "Blue" << "Alpha");
	channels_combo_->setToolTip( "Display channels");
	connect( channels_combo_, SIGNAL( activated( int)), image_viewer_, SLOT( change_channels( int)));
	channels_combo_->setMinimumSize( channels_combo_->sizeHint());
	horizontalLayout->addWidget( channels_combo_);
	QSize s = channels_combo_->sizeHint();
		
    mblur_btn_ = new QToolButton();
	mblur_btn_->setFocusPolicy( Qt::NoFocus);
	mblur_btn_->setIcon( QIcon( ":icons/mb.png"));
	mblur_btn_->setCheckable(true);
	mblur_btn_->setToolTip( "Motion blur on/off");
	mblur_btn_->setMaximumWidth( s.height());
	mblur_btn_->setMaximumHeight( s.height());	
	connect( mblur_btn_, SIGNAL( toggled( bool)), image_viewer_, SLOT( mblur_toggle( bool)));
	horizontalLayout->addWidget( mblur_btn_);

    show_checks_btn_ = new QToolButton();
	show_checks_btn_->setFocusPolicy( Qt::NoFocus);
	show_checks_btn_->setIcon( QIcon( ":icons/checks.png"));
    show_checks_btn_->setCheckable( true);
	show_checks_btn_->setToolTip( "Show checks background");
	show_checks_btn_->setMaximumWidth( s.height());
	show_checks_btn_->setMaximumHeight( s.height());	
	connect( show_checks_btn_, SIGNAL( toggled( bool)), image_viewer_, SLOT( checks_toggle( bool)));
	horizontalLayout->addWidget( show_checks_btn_);

	show_overlay_btn_ = new QToolButton();
	show_overlay_btn_->setFocusPolicy( Qt::NoFocus);
	show_overlay_btn_->setText( "O");
	show_overlay_btn_->setCheckable(true);
    show_overlay_btn_->setChecked( true);
	show_overlay_btn_->setToolTip( "Show overlay");
	show_overlay_btn_->setMaximumWidth( s.height());
	show_overlay_btn_->setMaximumHeight( s.height());	
	connect( show_overlay_btn_, SIGNAL( toggled( bool)), image_viewer_, SLOT( overlay_toggle( bool)));
	horizontalLayout->addWidget( show_overlay_btn_);

	QSpacerItem *horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	horizontalLayout->addItem( horizontalSpacer);

    setLayout( horizontalLayout);
}

void image_view_toolbar_t::update_widgets( node_t *n)
{
	if( n)
	{
		resolution_combo_->setEnabled( true);
		channels_combo_->setEnabled( true);
		mblur_btn_->setEnabled( true);
		show_checks_btn_->setEnabled( true);
		show_overlay_btn_->setEnabled( true);
	}
	else
	{
		resolution_combo_->setEnabled( false);
		channels_combo_->setEnabled( false);
		mblur_btn_->setEnabled( false);
		show_checks_btn_->setEnabled( false);
		show_overlay_btn_->setEnabled( false);		
	}
}

} // viewer
} // ui
} // ramen
