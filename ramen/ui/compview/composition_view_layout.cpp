// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.



#include<ramen/ui/compview/composition_view_layout.hpp>

#include<boost/foreach.hpp>

#include<adobe/algorithm/clamp.hpp>

#include<OpenEXR/ImathFun.h>

#include<ramen/assert.hpp>

#include<ramen/app/application.hpp>
#include<ramen/app/document.hpp>

#include<ramen/ui/compview/draw_pick_visitors.hpp>

namespace ramen
{
namespace ui
{

composition_view_layout_t::composition_view_layout_t() : rng_( 0) {}

void composition_view_layout_t::set_world( const Imath::Box2f& w)
{
	world_ = w;
}

void composition_view_layout_t::set_interest_point( const Imath::V2f& p) { interest_point_ = p;}

void composition_view_layout_t::place_node( node_t *n) const
{
	if( world_.isEmpty())
	{
		do_place_node( n, Imath::V2f( 0, 0));
		return;
	}
	
	Imath::V2f p;
	
	/*
	if( interest_point_ && world_.intersects( *interest_point_))
	{
		float u = Imath::lerpfactor( interest_point_->x, world_.min.x, world_.max.x);
		float v = Imath::lerpfactor( interest_point_->y, world_.min.y, world_.max.y);
	}
	else
	*/
	{
		float u = 0.5f;
		float v = 0.5f;
	
		float umargin = world_.size().x / 10;
		float vmargin = world_.size().y / 10;
	
		p = Imath::V2f( world_.min.x + umargin + ( u * ( world_.size().x - umargin - umargin)),
						world_.min.y + vmargin + ( v * ( world_.size().y - vmargin - vmargin)));
	}

	do_place_node( n, p);
}

void composition_view_layout_t::place_node_near_node( node_t *n, node_t *other) const
{	
    // TODO: this could be much better
    Imath::V2f p( other->location());
    p.x += 20;
    p.y += 60;
	do_place_node( n, p);
}

Imath::Box2f composition_view_layout_t::get_node_bbox( node_t *n) const
{
	Imath::Box2f box( n->location());
	size_node_visitor v;
	n->accept( v);
	box.max.x += v.size.x;
	box.max.y += v.size.y;
	return box;
}

bool composition_view_layout_t::box_intersects_any_node( const Imath::Box2f& box) const
{
    BOOST_FOREACH( node_t& n, app().document().composition().nodes())
    {
		box_pick_node_visitor visitor( box);
		n.accept( visitor);
		
		if( visitor.result)
			return true;
    }
	
	return false;
}

void composition_view_layout_t::do_place_node( node_t *n, const Imath::V2f& p) const
{
	Imath::V2f q( p);
	
	// init with a known position
	n->set_location( Imath::V2f( 0, 0));
	Imath::V2f size = get_node_bbox( n).max;

	if( box_intersects_any_node( Imath::Box2f( q, q + size)))
	{
	}
	
	n->set_location( q);
}

} // namespace
} // namespace
