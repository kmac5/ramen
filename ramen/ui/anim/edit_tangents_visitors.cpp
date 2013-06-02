// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/anim/edit_tangents_visitors.hpp>

#include<ramen/anim/float_curve.hpp>
#include<ramen/anim/shape_curve.hpp>

#include<ramen/ui/anim/anim_curves_view.hpp>

namespace ramen
{
namespace ui
{
namespace
{

void drag_left_tangent( const anim_curves_view_t& view, anim::float_curve_t *c, anim::float_key_t& key, const Imath::V2i& loc)
{
	float v = c->relative_to_absolute( key.value());
	Imath::V2i p( view.world_to_screen( Imath::V2f( key.time(), v)));
	Imath::V2f q( loc.x, loc.y);
	float slope;

	if( q.x >= p.x)
	{
		if( q.y >= p.y)
			slope = anim::float_key_t::max_slope();
		else
			slope = anim::float_key_t::min_slope();
	}
	else
	{
		Imath::V2f v( q.x - p.x, q.y - p.y);
		v.x /= -view.time_scale();
		v.y /=  view.value_scale();
		v.normalize();
		slope = v.y / v.x;
	}

	key.set_v0_tangent( slope / c->scale());
}

void drag_right_tangent( const anim_curves_view_t& view, anim::float_curve_t *c, anim::float_key_t& key, const Imath::V2i& loc)
{
	float v = c->relative_to_absolute( key.value());
	Imath::V2i p( view.world_to_screen( Imath::V2f( key.time(), v)));
	Imath::V2f q( loc.x, loc.y);
	float slope;

	if( q.x <= p.x)
	{
		if( q.y >= p.y)
			slope = anim::float_key_t::min_slope();
		else
			slope = anim::float_key_t::max_slope();
	}
	else
	{
		Imath::V2f v( q.x - p.x, q.y - p.y);
		v.x /=   view.time_scale();
		v.y /=  -view.value_scale();
		v.normalize();
		slope = v.y / v.x;
	}

	key.set_v1_tangent( slope / c->scale());
}

} // unnamed

drag_tangents_visitor::drag_tangents_visitor( const anim_curves_view_t& view, int index, bool left, const Imath::V2i& p, bool break_tangent) : view_( view)
{
	RAMEN_ASSERT( index >= 0);

	index_ = index;
	left_ = left;
	p_ = p;
	break_tangent_ = break_tangent;
}

void drag_tangents_visitor::operator()( anim::float_curve_t *c)
{
	anim::float_curve_t::iterator it( c->begin());
	it += index_;

	if( break_tangent_)
		it->set_tangent_continuity( false);
	
	if( left_)
		drag_left_tangent( view_, c, *it, p_);
	else
		drag_right_tangent( view_, c, *it, p_);

	c->recalc_tangents_and_coefficients( it);
}

set_autotangents_visitor::set_autotangents_visitor( anim::keyframe_t::auto_tangent_method m) : m_( m) {}
void set_autotangents_visitor::operator()( anim::float_curve_t *c)
{
	anim::float_curve_t::iterator it( c->begin());
	
	for( ; it != c->end(); ++it)
	{
		if( it->selected())
		{
		    it->set_tangent_continuity( false);

		    if( m_ == anim::keyframe_t::tangent_step)
		    {
				it->set_v0_auto_tangent( anim::keyframe_t::tangent_fixed);
				it->set_v1_auto_tangent( m_);
		    }
		    else
		    {
				it->set_v0_auto_tangent( m_);
				it->set_v1_auto_tangent( m_);

				if( m_ == anim::keyframe_t::tangent_smooth || m_ == anim::keyframe_t::tangent_flat)
				    it->set_tangent_continuity( true);
		    }
			
		}
	}
	
	c->recalc_tangents_and_coefficients();
}

void set_autotangents_visitor::operator()( anim::shape_curve2f_t *c)
{
	for( anim::shape_curve2f_t::iterator it( c->begin()); it != c->end(); ++it)
	{
		if( it->selected())
		{
		    if( m_ == anim::keyframe_t::tangent_step)
				it->set_v1_auto_tangent( m_);
		    else
		    {
				it->set_v0_auto_tangent( m_);
				it->set_v1_auto_tangent( m_);
		    }
			
		}
	}
}

} // namespace
} // namespace
