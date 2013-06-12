// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_INPUT_COMMAND_HPP
#define	RAMEN_IMAGE_INPUT_COMMAND_HPP

#include<ramen/undo/command.hpp>

#include<boost/tuple/tuple.hpp>

#include<ramen/nodes/image/input/input_node.hpp>

namespace ramen
{
namespace undo
{

class image_input_command_t : public undo::command_t
{
public:
	
	image_input_command_t( image::input_node_t& node, int proxy_level);

    virtual void undo();
    virtual void redo();
	
private:

	void swap_values();
	
	image::input_node_t& node_;
	int proxy_level_;
	
	image::input_clip_t old_clip_;
	
	boost::tuple<int,int,int,int> channels_;
	float aspect_;
};
	
} // undo
} // ramen

#endif
