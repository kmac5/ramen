// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/params/tab_group_param.hpp>

#include<boost/bind.hpp>
#include<boost/foreach.hpp>
#include<boost/range/algorithm/for_each.hpp>

#include<QTabWidget>

#include<ramen/ui/inspector/inspector.hpp>

namespace ramen
{

tab_group_param_t::tab_group_param_t() : composite_param_t() {}
tab_group_param_t::tab_group_param_t( const std::string& name) : composite_param_t( name) {}

tab_group_param_t::tab_group_param_t( const tab_group_param_t& other) : composite_param_t( other), tab_(0) {}

QWidget *tab_group_param_t::do_create_widgets()
{
    tab_ = new QTabWidget();

    BOOST_FOREACH( param_t& p, params())
    {
		QWidget *w = p.create_widgets();

		if( w)
		    tab_->addTab( w, p.name().c_str());
    }

    tab_->setCurrentIndex( 0);
    return tab_;
}

void tab_group_param_t::do_enable_widgets( bool e)
{
    if( tab_)
		tab_->setEnabled( e);
}

void tab_group_param_t::do_create_tracks( anim::track_t *parent)
{
    boost::range::for_each( params(), boost::bind( &param_t::create_tracks, _1, parent));
}

} // namespace
