// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_ANIM_PICK_CURVE_VISITORS_HPP
#define	RAMEN_UI_ANIM_PICK_CURVE_VISITORS_HPP

#include<ramen/ui/anim/draw_curves_visitors.hpp>

namespace ramen
{
namespace ui
{

struct pick_tangents_visitor : public boost::static_visitor<>
{
    pick_tangents_visitor( const anim_curves_view_t& view, const Imath::V2i& p);
	
    void operator()( const anim::float_curve_t *c);

	template<class K>
	void operator()( const anim::shape_curve_t<K> *c) { key_index = -1;}
	
    int key_index;
    bool left;

private:

	const anim_curves_view_t& view_;
    Imath::V2i p_;
};

struct pick_keyframe_visitor : public boost::static_visitor<>
{
    pick_keyframe_visitor( const anim_curves_view_t& view, const Imath::V2i& p);

    void operator()( const anim::float_curve_t *c);
    void operator()( const anim::shape_curve2f_t *c);

    int key_index;

private:

	const anim_curves_view_t& view_;
    Imath::V2i p_;
};

struct box_pick_keyframes_visitor : public boost::static_visitor<>
{
    box_pick_keyframes_visitor( const anim_curves_view_t& view, const Imath::Box2i& box);

    void operator()( const anim::float_curve_t *c);
    void operator()( const anim::shape_curve2f_t *c);

    bool any_picked;

private:

	const anim_curves_view_t& view_;
    Imath::Box2i box_;
};

} // namespace
} // namespace

#endif
