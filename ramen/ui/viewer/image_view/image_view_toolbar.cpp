// Copyright (c) 2010 Esteban Tovagliari



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

	/*
	proxy_combo_ = new QComboBox();
	proxy_combo_->setFocusPolicy( Qt::NoFocus);
	proxy_combo_->insertItems( 0, QStringList() << "Full Res" << "Proxy 1" << "Proxy 2");
	proxy_combo_->setMinimumSize( proxy_combo_->sizeHint());
	proxy_combo_->setToolTip( "Proxy level");
	connect( proxy_combo_, SIGNAL( activated( int)), image_viewer_, SLOT( change_proxy_level(int)));
	horizontalLayout->addWidget( proxy_combo_);
	*/
	
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
	
	/*
    aspect_btn_ = new QToolButton();
	aspect_btn_->setFocusPolicy( Qt::NoFocus);
	aspect_btn_->setIcon( QIcon( ":icons/aspect.png"));
	aspect_btn_->setCheckable(true);
	aspect_btn_->setToolTip( "Aspect ratio on/off");
	aspect_btn_->setMaximumWidth( s.height());
	aspect_btn_->setMaximumHeight( s.height());	
	connect( aspect_btn_, SIGNAL( toggled( bool)), image_viewer_, SLOT( aspect_toggle( bool)));
	horizontalLayout->addWidget( aspect_btn_);
	*/
	
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

	/*
	show_interest_btn_ = new QToolButton();
	show_interest_btn_->setFocusPolicy( Qt::NoFocus);
	show_interest_btn_->setText( "I");
	show_interest_btn_->setCheckable( true);
	show_interest_btn_->setEnabled( false);
	show_interest_btn_->setToolTip( "Show region of interest");
	show_interest_btn_->setMaximumWidth( s.height());
	show_interest_btn_->setMaximumHeight( s.height());	
	connect( show_interest_btn_, SIGNAL( toggled( bool)), image_viewer_, SLOT( roi_toggle( bool)));
    horizontalLayout->addWidget( show_interest_btn_);
	*/

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
		//aspect_btn_->setEnabled( true);
		mblur_btn_->setEnabled( true);
		show_checks_btn_->setEnabled( true);
		//show_interest_btn_->setEnabled( true);
		show_overlay_btn_->setEnabled( true);
	}
	else
	{
		resolution_combo_->setEnabled( false);
		channels_combo_->setEnabled( false);
		//aspect_btn_->setEnabled( false);
		mblur_btn_->setEnabled( false);
		show_checks_btn_->setEnabled( false);
		//show_interest_btn_->setEnabled( false);
		show_overlay_btn_->setEnabled( false);		
	}
}

} // namespace
} // namespace
} // namespace
