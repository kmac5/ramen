// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/nodes/image/key/keyer3d/keyer3d_commands.hpp>

#include<ramen/nodes/image/key/keyer3d/keyer3d_node.hpp>

namespace ramen
{
namespace undo
{

keyer3d_command_t::keyer3d_command_t( image::keyer3d_node_t& node) : undo::command_t( "Keyer sample"), node_( node), old_( node_.keyer())
{
    set_done( true);
}

void keyer3d_command_t::undo()
{
	node_.keyer().swap( old_);
	node_.update_clusters_params();
	node_.notify();
    undo::command_t::undo();
}

void keyer3d_command_t::redo()
{
	node_.keyer().swap( old_);
	node_.update_clusters_params();
	node_.notify();
    undo::command_t::undo();
}

} // namespace
} // namespace
