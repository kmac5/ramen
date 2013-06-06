// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/nodes/image/track/tracker_toolbar.hpp>

#include<QHBoxLayout>
#include<QSpacerItem>
#include<QToolButton>
#include<QFrame>

#include<ramen/nodes/image/track/tracker_node.hpp>

#include<ramen/app/application.hpp>
#include<ramen/ui/user_interface.hpp>

namespace ramen
{

tracker_toolbar_t::tracker_toolbar_t( image::tracker_node_t& parent) : parent_( parent), QWidget()
{
    setMinimumSize( 0, app().ui()->viewer_toolbar_height());
    setMaximumSize( QWIDGETSIZE_MAX, app().ui()->viewer_toolbar_height());

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins( 0, 0, 0, 0);

    QToolButton *track_back_but = new QToolButton( this);
    track_back_but->setIcon( QIcon( ":icons/play_back.png"));
    track_back_but->setFocusPolicy( Qt::NoFocus);
    track_back_but->setToolTip( "Track backwards");
	connect( track_back_but, SIGNAL( clicked()), this, SLOT( track_back()));
    layout->addWidget( track_back_but);

    QToolButton *frame_back_but = new QToolButton( this);
    frame_back_but->setIcon( QIcon( ":icons/prev_frame.png"));
    frame_back_but->setFocusPolicy( Qt::NoFocus);
    frame_back_but->setToolTip( "Track one frame backwards");
    connect( frame_back_but, SIGNAL( clicked()), this, SLOT( track_one_back()));
    layout->addWidget( frame_back_but);

    QToolButton *frame_fwd_but = new QToolButton( this);
    frame_fwd_but->setIcon( QIcon( ":icons/next_frame.png"));
    frame_fwd_but->setFocusPolicy( Qt::NoFocus);
    frame_fwd_but->setToolTip( "Track one frame forward");
    connect( frame_fwd_but, SIGNAL( clicked()), this, SLOT( track_one_fwd()));
    layout->addWidget( frame_fwd_but);

    QToolButton *track_fwd_but = new QToolButton( this);
    track_fwd_but->setIcon( QIcon( ":icons/play.png"));
    track_fwd_but->setFocusPolicy( Qt::NoFocus);
    track_fwd_but->setToolTip( "Track forward");
    connect( track_fwd_but, SIGNAL( clicked()), this, SLOT( track_fwd()));
    layout->addWidget( track_fwd_but);
	
    QFrame *separator = new QFrame();
    separator->setFrameStyle( QFrame::VLine | QFrame::Raised);
    separator->setLineWidth( 1);
    layout->addWidget( separator);

    QToolButton *grab_but = new QToolButton( this);
	grab_but->setText( "Grab");
    grab_but->setFocusPolicy( Qt::NoFocus);
    grab_but->setToolTip( "Grab");
    connect( grab_but, SIGNAL( clicked()), this, SLOT( grab_references()));
    layout->addWidget( grab_but);
	
    QSpacerItem *s = new QSpacerItem( 30, 30, QSizePolicy::Expanding, QSizePolicy::Minimum);
    layout->addItem( s);

    setLayout( layout);
}

void tracker_toolbar_t::grab_references()	{ parent_.grab_references();}
void tracker_toolbar_t::track_back()		{ parent_.track_backwards();}
void tracker_toolbar_t::track_fwd()			{ parent_.track_forward();}
void tracker_toolbar_t::track_one_back()	{ parent_.track_one_backwards();}
void tracker_toolbar_t::track_one_fwd()		{ parent_.track_one_forward();}

} // namespace
