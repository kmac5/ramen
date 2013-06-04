// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_ANIM_EDIT_TANGENTS_VISITORS_HPP
#define	RAMEN_UI_ANIM_EDIT_TANGENTS_VISITORS_HPP

#include<OpenEXR/ImathVec.h>

#include<ramen/anim/any_curve.hpp>
#include<ramen/anim/float_curve.hpp>
#include<ramen/anim/shape_curve.hpp>

#include<ramen/ui/anim/anim_curves_view_fwd.hpp>

namespace ramen
{
namespace ui
{

struct drag_tangents_visitor : public boost::static_visitor<>
{
	drag_tangents_visitor( const anim_curves_view_t& view, int index, bool left, const Imath::V2i& p, bool break_tangent = false);
	void operator()( anim::float_curve_t *c);
	void operator()( anim::shape_curve2f_t *c) {}

private:

	const anim_curves_view_t& view_;
	Imath::V2i p_;
	int index_;
	bool left_;
	bool break_tangent_;
};

struct can_set_autotangents_visitor : public boost::static_visitor<>
{
	can_set_autotangents_visitor( anim::keyframe_t::auto_tangent_method m) : m_( m) {}
	void operator()( const anim::float_curve_t *c)	{ result = true;}
	void operator()( anim::shape_curve2f_t *c)
	{
		result = ( m_ == anim::keyframe_t::tangent_step || m_ == anim::keyframe_t::tangent_linear);
	}
	
	bool result;
	
private:

	anim::keyframe_t::auto_tangent_method m_;	
};

struct set_autotangents_visitor : public boost::static_visitor<>
{
	set_autotangents_visitor( anim::keyframe_t::auto_tangent_method m);
	void operator()( anim::float_curve_t *c);
	void operator()( anim::shape_curve2f_t *c);
		
private:

	anim::keyframe_t::auto_tangent_method m_;	
};

struct can_set_extrapolation_visitor : public boost::static_visitor<>
{
	can_set_extrapolation_visitor( anim::extrapolation_method m) : m_( m) {}

	template<class K>
	void operator()( anim::curve_t<K> *c)		{ result = false;}
	
	void operator()( anim::float_curve_t *c)	{ result = true;}
	
	bool result;

private:

	anim::extrapolation_method m_;
};

struct set_extrapolation_visitor : public boost::static_visitor<>
{
	set_extrapolation_visitor( anim::extrapolation_method m) : m_( m) {}
	
	template<class K>
	void operator()( anim::curve_t<K> *c)
	{
		RAMEN_ASSERT( 0 && "Can't set extrapolation for non float curves");
	}

	void operator()( anim::float_curve_t *c)
	{
		c->set_extrapolation( m_);
	}

private:

	anim::extrapolation_method m_;
};

} // namespace
} // namespace

#endif
