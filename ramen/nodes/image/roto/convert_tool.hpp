// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_ROTO_CONVERT_TOOL_HPP
#define	RAMEN_ROTO_CONVERT_TOOL_HPP

#include<ramen/nodes/image/roto/tool.hpp>

namespace ramen
{
namespace roto
{

class convert_tool_t : public tool_t
{
public:

	convert_tool_t( image::roto_node_t& parent);

    virtual void draw_overlay( const ui::paint_event_t& event) const;
	
	virtual void mouse_press_event( const ui::mouse_press_event_t& event);
	virtual void mouse_drag_event( const ui::mouse_drag_event_t& event);
	virtual void mouse_release_event( const ui::mouse_release_event_t& event);
	
private:

    bool box_pick_mode_;
	Imath::Box2f box_;
};

} // namespace
} // namespace

#endif
