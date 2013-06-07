// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/roto/tool.hpp>

#include<boost/foreach.hpp>

#include<ramen/nodes/image/roto/shape.hpp>

namespace ramen
{
namespace roto
{

tool_t::tool_t( image::roto_node_t& parent) : ui::tool_t(), parent_( parent) {}

triple_t *tool_t::pick_point( shape_t *s, const ui::mouse_event_t& event)
{
	Imath::V2f offset = s->offset();
	
	if( s->inv_global_xform())
	{
		Imath::V2f p( event.wpos.x / event.aspect_ratio, event.wpos.y);
		p = p * s->inv_global_xform().get();
	
		BOOST_FOREACH( triple_t& t, s->triples())
		{
			if( inside_pick_distance( t.p1() + offset, p, event.pixel_scale))
				return &t;
		}
	}
	
	return 0;
}

void tool_t::select_points_in_box( shape_t *s, const Imath::Box2f& box, const ui::mouse_event_t& event)
{
	Imath::Box2f bbox( box);
	bbox.min.x /= event.aspect_ratio;
	bbox.max.x /= event.aspect_ratio;

	Imath::V2f offset = s->offset();
	
	BOOST_FOREACH( triple_t& t, s->triples())
	{
		Imath::V2f p( ( t.p1() + offset) * s->global_xform());
		
		if( bbox.intersects( p))
			t.select( true);
	}
}

} // namespace
} // namespace
