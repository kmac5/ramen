// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_NODE_MENU_HPP
#define	RAMEN_UI_NODE_MENU_HPP

#include<vector>
#include<string>

class QAction;
class QMenu;

namespace ramen
{
namespace ui
{

class node_menu_t
{
public:

    node_menu_t( const std::string& name);

    const std::string& name() const { return name_;}
    QMenu *menu() { return menu_;}

    std::vector<QMenu*>& submenus() { return submenus_;}

    void add_submenu( const std::string& name);

    void add_action( const std::string& submenu, QAction *action);

private:

    std::vector<QMenu*>::iterator find_submenu( const std::string& name);

    std::string name_;
    QMenu *menu_;
    std::vector<QMenu*> submenus_;
};

} // namespace
} // namespace

#endif
