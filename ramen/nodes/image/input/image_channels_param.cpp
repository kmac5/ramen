// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/input/image_channels_param.hpp>

#include<algorithm>

#include<boost/range/algorithm/find.hpp>

#include<QPushButton>
#include<QVBoxLayout>
#include<QLabel>

#include<ramen/app/application.hpp>

#include<ramen/params/param_set.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/inspector/inspector.hpp>

namespace ramen
{

image_channels_param_t::image_channels_param_t() : static_param_t( std::string())
{
	value().assign( tuple4i_t( 0, 1, 2, 3));
}

image_channels_param_t::image_channels_param_t( const image_channels_param_t& other) : static_param_t( other)
{
    red_combo_ = green_combo_ = blue_combo_ = alpha_combo_ = 0;
	channel_list_ = other.channel_list_;
}

void image_channels_param_t::clear_channel_list()
{
	channel_list_.clear();
	value().assign( tuple4i_t( 0, 1, 2, 3));
	
	if( red_combo_)
	{
		update_popup( red_combo_   , 0);
		update_popup( green_combo_ , 1);
		update_popup( blue_combo_  , 2);
		update_popup( alpha_combo_ , 3);
	}
}

void image_channels_param_t::set_channel_list( const std::vector<std::string>& channels)
{
	if( !channel_list_.empty())
	{
		std::string ch0, ch1, ch2, ch3;
		get_channel_names( ch0, ch1, ch2, ch3);
		channel_list_ = channels;
		set_channels( ch0, ch1, ch2, ch3);
	}
	else
	{
		channel_list_ = channels;
		value().assign( tuple4i_t( 0, 1, 2, 3));
	}

	tuple4i_t ch( get_value<tuple4i_t>( *this));
	
	if( red_combo_)
	{
		update_popup( red_combo_   , boost::get<0>( ch));
		update_popup( green_combo_ , boost::get<1>( ch));
		update_popup( blue_combo_  , boost::get<2>( ch));
		update_popup( alpha_combo_ , boost::get<3>( ch));
	}
}

void image_channels_param_t::get_channel_names( std::string& red, std::string& green,
												std::string& blue, std::string& alpha) const
{
	tuple4i_t ch( get_value<tuple4i_t>( *this));
	
	if( boost::get<0>( ch) < channel_list_.size())
		red = channel_list_[ boost::get<0>( ch)];
	else
		red = "R";
	
	if( boost::get<1>( ch) < channel_list_.size())
		green = channel_list_[ boost::get<1>( ch)];
	else
		green = "G";

	if( boost::get<2>( ch) < channel_list_.size())
		blue = channel_list_[ boost::get<2>( ch)];
	else
		blue = "B";

	if( boost::get<3>( ch) < channel_list_.size())
		alpha = channel_list_[ boost::get<3>( ch)];
	else
		alpha = "A";
}

void image_channels_param_t::set_channels( const tuple4i_t& ch)
{ 
	value().assign( ch);
}

void image_channels_param_t::set_channels( const std::string& red, const std::string& green,
											const std::string& blue, const std::string& alpha)
{
	RAMEN_ASSERT( !channel_list_.empty());
	
    int c0, c1, c2, c3;
    std::vector<std::string>::const_iterator it;

    // red
    it = boost::range::find( channel_list_, red);

    if( it != channel_list_.end())
		c0 = it - channel_list_.begin();
    else
		c0 = channel_list_.size();

    // green
    it = boost::range::find( channel_list_, green);

    if( it != channel_list_.end())
		c1 = it - channel_list_.begin();
    else
		c1 = channel_list_.size();

    // blue
    it = boost::range::find( channel_list_, blue);

    if( it != channel_list_.end())
		c2 = it - channel_list_.begin();
    else
		c2 = channel_list_.size();

    // alpha
    it = boost::range::find( channel_list_, alpha);

    if( it != channel_list_.end())
		c3 = it - channel_list_.begin();
    else
		c3 = channel_list_.size();

    tuple4i_t ch( c0, c1, c2, c3);
    value().assign( ch);
}

// ui
QWidget *image_channels_param_t::do_create_widgets()
{
    QWidget *top = new QWidget();
    int current_height = 0;

    // popups
    {
		QLabel *label = new QLabel( top);
		red_combo_ = new QComboBox( top);
		red_combo_->setFocusPolicy( Qt::NoFocus);
		QSize s = red_combo_->sizeHint();

		label->move( 0, current_height);
		label->resize( app().ui()->inspector().left_margin() - 5, s.height());
		label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
		label->setText( "Red");

		red_combo_->move( app().ui()->inspector().left_margin(), current_height);
		red_combo_->resize( app().ui()->inspector().width() - app().ui()->inspector().left_margin() - 10, s.height());
		connect( red_combo_, SIGNAL( currentIndexChanged( int)), this, SLOT( channel_picked( int)));
		current_height = s.height() + 5;
    }

    {
		QLabel *label = new QLabel( top);
		green_combo_ = new QComboBox( top);
		green_combo_->setFocusPolicy( Qt::NoFocus);

		QSize s = green_combo_->sizeHint();

		label->move( 0, current_height);
		label->resize( app().ui()->inspector().left_margin() - 5, s.height());
		label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
		label->setText( "Green");

		green_combo_->move( app().ui()->inspector().left_margin(), current_height);
		green_combo_->resize( app().ui()->inspector().width() - app().ui()->inspector().left_margin() - 10, s.height());
		connect( green_combo_, SIGNAL( currentIndexChanged( int)), this, SLOT( channel_picked( int)));
		current_height += s.height() + 5;
    }

    {
		QLabel *label = new QLabel( top);
		blue_combo_ = new QComboBox( top);
		blue_combo_->setFocusPolicy( Qt::NoFocus);

		QSize s = blue_combo_->sizeHint();

		label->move( 0, current_height);
		label->resize( app().ui()->inspector().left_margin() - 5, s.height());
		label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
		label->setText( "Blue");

		blue_combo_->move( app().ui()->inspector().left_margin(), current_height);
		blue_combo_->resize( app().ui()->inspector().width() - app().ui()->inspector().left_margin() - 10, s.height());
		connect( blue_combo_, SIGNAL( currentIndexChanged( int)), this, SLOT( channel_picked( int)));
		current_height += s.height() + 5;
    }

    {
		QLabel *label = new QLabel( top);
		alpha_combo_ = new QComboBox( top);
		alpha_combo_->setFocusPolicy( Qt::NoFocus);

		QSize s = alpha_combo_->sizeHint();

		label->move( 0, current_height);
		label->resize( app().ui()->inspector().left_margin() - 5, s.height());
		label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
		label->setText( "Alpha");

		alpha_combo_->move( app().ui()->inspector().left_margin(), current_height);
		alpha_combo_->resize( app().ui()->inspector().width() - app().ui()->inspector().left_margin() - 10, s.height());
		connect( alpha_combo_, SIGNAL( currentIndexChanged( int)), this, SLOT( channel_picked( int)));
		current_height += s.height() + 5;
    }

    top->setMinimumSize( app().ui()->inspector().width(), current_height);
    top->setMaximumSize( app().ui()->inspector().width(), current_height);
    top->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);

	tuple4i_t ch( get_value<tuple4i_t>( *this));
	update_popup( red_combo_   , boost::get<0>( ch));
	update_popup( green_combo_ , boost::get<1>( ch));
	update_popup( blue_combo_  , boost::get<2>( ch));
	update_popup( alpha_combo_ , boost::get<3>( ch));
    return top;
}

void image_channels_param_t::do_update_widgets()
{
	if( red_combo_)
	{
		tuple4i_t ch( get_value<tuple4i_t>( *this));

		red_combo_->blockSignals( true);
		green_combo_->blockSignals( true);
		blue_combo_->blockSignals( true);
		alpha_combo_->blockSignals( true);

		red_combo_->setCurrentIndex( boost::get<0>( ch));
		green_combo_->setCurrentIndex( boost::get<1>( ch));
		blue_combo_->setCurrentIndex( boost::get<2>( ch));
		alpha_combo_->setCurrentIndex( boost::get<3>( ch));
		
		red_combo_->blockSignals( false);
		green_combo_->blockSignals( false);
		blue_combo_->blockSignals( false);
		alpha_combo_->blockSignals( false);		
	}
}

void image_channels_param_t::do_enable_widgets( bool e)
{
	if( red_combo_)
	{
		red_combo_->setEnabled( e);
		green_combo_->setEnabled( e);
		blue_combo_->setEnabled( e);
		alpha_combo_->setEnabled( e);
	}
}

void image_channels_param_t::update_popup( QComboBox *popup, int value)
{
    popup->blockSignals( true);
	popup->clear();
	
    if( !channel_list_.empty())
    {
		for( int i = 0; i < channel_list_.size(); ++i)
			popup->addItem( channel_list_[i].c_str());

		popup->addItem( "None");
		popup->setEnabled( true);
    }
    else
    {
		popup->addItem( "Red");
		popup->addItem( "Green");
		popup->addItem( "Blue");
		popup->addItem( "Alpha");
		popup->setEnabled( false);
    }

    popup->setCurrentIndex( value);
    popup->blockSignals( false);
}

void image_channels_param_t::channel_picked( int index)
{
	tuple4i_t ch( get_value<tuple4i_t>( *this));

    if( QObject::sender() == red_combo_)
		boost::get<0>( ch) = index;

    if( QObject::sender() == green_combo_)
		boost::get<1>( ch) = index;

    if( QObject::sender() == blue_combo_)
		boost::get<2>( ch) = index;

    if( QObject::sender() == alpha_combo_)
		boost::get<3>( ch) = index;

    param_set()->begin_edit();
    param_set()->add_command( this);
    value().assign( ch);
	emit_param_changed( user_edited);
    param_set()->end_edit();
}

} // namespace
