// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/params/proportional_param.hpp>

namespace ramen
{

Imath::V3f proportional_param_t::proportional_factor = Imath::V3f( 0, 0, 0);

proportional_param_t::proportional_param_t( const std::string& name) : numeric_param_t( name) {}

proportional_param_t::proportional_param_t( const proportional_param_t& other) : numeric_param_t( other)
{
    prop_button_ = 0;
}

void proportional_param_t::set_proportional( bool p)
{
    if( p)
		set_flags( flags() | proportional_bit);
    else
		set_flags( flags() & ~proportional_bit);
}

bool proportional_param_t::proportional_checked() const
{
    if( prop_button_)
        return prop_button_->isChecked();

    return false;
}

void proportional_param_t::create_proportional_button( QWidget *parent, int height)
{
    prop_button_ = new QToolButton( parent);
    prop_button_->setIcon( QIcon( ":icons/link.png"));
    prop_button_->setCheckable( true);
    prop_button_->setChecked( true);
    prop_button_->setEnabled( enabled());
	prop_button_->resize( height, height);
	connect( prop_button_, SIGNAL( toggled( bool)), this, SLOT( proportional_toggle(bool)));
}

void proportional_param_t::proportional_toggle( bool state)
{
	if( state)
		prop_button_->setIcon( QIcon( ":icons/link.png"));
	else
		prop_button_->setIcon( QIcon( ":icons/unlink.png"));
}

} // namespace
