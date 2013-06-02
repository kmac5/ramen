// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/ui/node_menu.hpp>

#include<QMenu>
#include<QAction>

namespace ramen
{
namespace ui
{

node_menu_t::node_menu_t( const std::string& name) : name_( name), menu_( 0)
{
    menu_ = new QMenu( name.c_str());
}

void node_menu_t::add_submenu( const std::string& name)
{
    submenus_.push_back( menu_->addMenu( name.c_str()));
}

void node_menu_t::add_action( const std::string& submenu, QAction *action)
{
    std::vector<QMenu*>::iterator menu_it = find_submenu( submenu);

    if( menu_it == submenus_.end())
    {
        submenus_.push_back( menu_->addMenu( QString( submenu.c_str())));
        menu_it = submenus_.end() - 1;
    }

    (*menu_it)->addAction( action);
}

std::vector<QMenu*>::iterator node_menu_t::find_submenu( const std::string& name)
{
    for( std::vector<QMenu*>::iterator it( submenus_.begin()); it != submenus_.end(); ++it)
    {
	if( (*it)->title().toStdString() == name)
	    return it;
    }

    return submenus_.end();
}

} // namespace
} // namespace
