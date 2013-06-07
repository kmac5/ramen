// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_ROTO_TOOL_HPP
#define	RAMEN_ROTO_TOOL_HPP

#include<ramen/ui/tool.hpp>

#include<OpenEXR/ImathMatrix.h>

#include<ramen/bezier/algorithm.hpp>

#include<ramen/nodes/image/roto/shape_fwd.hpp>
#include<ramen/nodes/image/roto/triple.hpp>
#include<ramen/nodes/image/roto/roto_node_fwd.hpp>

namespace ramen
{
namespace roto
{

class tool_t : public ui::tool_t
{
public:

    tool_t( image::roto_node_t& parent);

    const image::roto_node_t& parent() const	{ return parent_;}
    image::roto_node_t& parent()				{ return parent_;}

protected:
	
	triple_t *pick_point( shape_t *s, const ui::mouse_event_t& event);
	void select_points_in_box( shape_t *s, const Imath::Box2f& box, const ui::mouse_event_t& event);
	
private:
	
    image::roto_node_t& parent_;
};

} // namespace
} // namespace

#endif
