// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/params/inspector_param.hpp>

#include<boost/bind.hpp>

#include<QFrame>

#include<ramen/app/application.hpp>

#include<ramen/params/parameterised.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/inspector/inspector.hpp>
#include<ramen/ui/inspector/panel.hpp>

namespace ramen
{

inspector_param_t::inspector_param_t( const std::string& id) : param_t()
{
	set_static( true);
	set_id( id);
	contents_ = 0;
}

inspector_param_t::inspector_param_t( const inspector_param_t& other) : param_t( other), widget_( 0), contents_( 0)
{
}

inspector_param_t::~inspector_param_t() { connection_.disconnect();}

void inspector_param_t::set_parameterised( parameterised_t *p)
{
	if( contents_)
	{
		connection_.disconnect();
		contents_ = 0;
		
		if( widget_)
	        widget_->clear_contents();
	}

	if( p)
	{
		contents_ = p;
		connection_ = p->deleted.connect( boost::bind( &inspector_param_t::parameterised_deleted, this, _1));
	
		if( widget_)
		{
			QWidget *w = app().ui()->inspector().panel_factory().create_panel( p)->second->widget();
			widget_->set_contents( w);
		}
	}
}

QWidget *inspector_param_t::do_create_widgets()
{
	widget_ = new ui::container_widget_t();
    widget_->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred);
    widget_->setMinimumSize( app().ui()->inspector().width()+16, 0);
    widget_->setMaximumSize( app().ui()->inspector().width()+16, QWIDGETSIZE_MAX);
    widget_->setContentsMargins( 0, 0, 0, 0);
	
	if( contents_)
	{
		QWidget *w = app().ui()->inspector().panel_factory().create_panel( contents_)->second->widget();
		widget_->set_contents( w);
	}
	
	return widget_;
}

void inspector_param_t::parameterised_deleted( parameterised_t *p)
{
	if( p == contents_)
		set_parameterised( 0);
}

} // namespace
