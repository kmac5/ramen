// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_INSPECTOR_PANEL_FACTORY_HPP
#define	RAMEN_UI_INSPECTOR_PANEL_FACTORY_HPP

#include<map>

#include<boost/noncopyable.hpp>

#include<ramen/params/parameterised_fwd.hpp>
#include<ramen/ui/inspector/panel_fwd.hpp>

namespace ramen
{
namespace ui
{

class panel_factory_t : boost::noncopyable
{
public:

    panel_factory_t();
	~panel_factory_t();

	typedef std::map<parameterised_t*,panel_t*>::iterator iterator;

	iterator begin()	{ return panels_.begin();}
	iterator end()		{ return panels_.end();}

    iterator create_panel( parameterised_t *p);
    void delete_panel( parameterised_t *p);

private:

    void do_create_panel( parameterised_t *p);

    std::map<parameterised_t*,panel_t*> panels_;
    std::map<parameterised_t*,panel_t*>::iterator current_;
};

} // namespace
} // namespace

#endif
