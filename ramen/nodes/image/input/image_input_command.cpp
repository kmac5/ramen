// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/input/image_input_command.hpp>

#include<ramen/app/application.hpp>
#include<ramen/app/document.hpp>

namespace ramen
{
namespace undo
{

image_input_command_t::image_input_command_t( image::input_node_t& node, int proxy_level) : undo::command_t( "Param changed"), node_( node)
{
	proxy_level_ = proxy_level;
	old_clip_ = node.clips()[proxy_level_];
	
	if( proxy_level_ == 0)
	{
		channels_ = node.get_channels();
		aspect_ = node.get_aspect_param_value();
	}
	
    set_done( true);
}

void image_input_command_t::undo()
{
	node_.clips()[proxy_level_].swap( old_clip_);	
	node_.create_reader( proxy_level_);
	node_.set_frame( app().document().composition().frame());
	
	if( proxy_level_ == 0)
	{
		swap_values();
		node_.update_widgets();
	}
	
	node_.notify();
    undo::command_t::undo();
}

void image_input_command_t::redo()
{
	node_.clips()[proxy_level_].swap( old_clip_);
	node_.create_reader( proxy_level_);
	node_.set_frame( app().document().composition().frame());

	if( proxy_level_ == 0)
	{
		swap_values();
		node_.update_widgets();
	}

	node_.notify();
    undo::command_t::redo();
}

void image_input_command_t::swap_values()
{
	boost::tuple<int,int,int,int> tmp_ch = node_.get_channels();
	float tmp_aspect = node_.get_aspect_param_value();
	
	node_.set_channels( channels_);
	node_.set_aspect_param_value( aspect_);
	
	channels_ = tmp_ch;
	aspect_ = tmp_aspect;
}

} // namespace
} // namespace
