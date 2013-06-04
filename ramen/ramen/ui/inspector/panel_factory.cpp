// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/python/python.hpp>

#include<ramen/ui/inspector/panel_factory.hpp>

#include<boost/bind.hpp>

#include<ramen/params/parameterised.hpp>

#include<ramen/ui/inspector/panel.hpp>

namespace ramen
{
namespace ui
{

panel_factory_t::panel_factory_t() {}

panel_factory_t::~panel_factory_t()
{
	//RAMEN_ASSERT( panels_.empty());
}

panel_factory_t::iterator panel_factory_t::create_panel( parameterised_t *p)
{
	RAMEN_ASSERT( p != 0);
	
	iterator it = panels_.find( p);

	if( it == panels_.end())
	{
		do_create_panel( p);
		it = panels_.find( p);
	}

	return it;
}

void panel_factory_t::delete_panel( parameterised_t *p)
{
    std::map<parameterised_t*, panel_t*>::iterator it( panels_.find( p));

    if( it != panels_.end())
    {
        delete it->second;
        panels_.erase( p);
    }
}

void panel_factory_t::do_create_panel( parameterised_t *p)
{
    panel_t *panel = new panel_t( p);
    panels_[ p] = panel;
	boost::signals2::connection c = p->deleted.connect( boost::bind( &panel_factory_t::delete_panel, this, _1));
	panel->set_connection( c);
}

} // namespace
} // namespace
