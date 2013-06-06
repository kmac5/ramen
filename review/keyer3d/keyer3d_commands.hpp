// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_KEYER3D_COMMANDS_HPP
#define	RAMEN_KEYER3D_COMMANDS_HPP

#include<ramen/undo/command.hpp>

#include<vector>

#include<ramen/nodes/image/key/keyer3d/keyer3d_node_fwd.hpp>
#include<ramen/nodes/image/key/keyer3d/km_keyer.hpp>

namespace ramen
{
namespace undo
{

class keyer3d_command_t : public undo::command_t
{
public:
	
	keyer3d_command_t( image::keyer3d_node_t& node);

    virtual void undo();
    virtual void redo();
	
private:

	image::keyer3d_node_t& node_;
	keyer3d::km_keyer_t old_;
};
	
} // undo
} // ramen

#endif
