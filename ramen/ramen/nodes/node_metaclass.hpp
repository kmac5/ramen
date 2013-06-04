// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_NODE_METACLASS_HPP
#define RAMEN_NODE_METACLASS_HPP

#include<ramen/config.hpp>

#include<ramen/nodes/node_fwd.hpp>

#include<string>

#include<boost/function.hpp>

namespace ramen
{

/*!
\ingroup nodes
\brief This class contains information about a node class like ids, menus, etc.
*/
class RAMEN_API node_metaclass_t
{
public:

    node_metaclass_t();

    /// Unique id for this node
    std::string id;

    /// Major version
    int major_version;

    /// Minor version
    int minor_version;

    /// Does this node appears on menus?
    bool ui_visible;

    /// Menu, for example "Image"
    std::string menu;

    /// Submenu, for example "Filter"
    std::string submenu;

    /// Menu item, for example "Blur"
    std::string menu_item;
    
    /// Hotkey for menu item
    std::string hotkey;

    /// Short help for this node
    const char *help;

    /*!
		\brief Called before the first instance of this node is created.
		Use this function to allocate resources shared by all nodes of this class
	*/
	void (*init)();

	/*!
		\brief Called at application exit if an instance of this node was created.
		Use this function to deallocate resources shared by all nodes of this class
	*/
	void (*cleanup)();

	/*!
		\brief Creates a new node

		This function is called to create an instance of this node.
		It can also return a null pointer, or even better, throw an exception
		derived from std::exception if the node couldn't be created.
	*/
	node_t *(*create)();

	/*!
		\brief Creates a new node

		This function is called to create an instance of this node, when
		the user interface is active. It can show dialog boxes, file dialogs, ...
		It can also return a null pointer, or even better, throw an exception
		derived from std::exception if the node couldn't be created.
	*/
	node_t *(*create_gui)();

// private:
	
	bool first_time_;
};

} // namespace

#endif
