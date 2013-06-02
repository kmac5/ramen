// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_ANIM_DRAW_CURVE_VISITORS_HPP
#define	RAMEN_UI_ANIM_DRAW_CURVE_VISITORS_HPP

#include<OpenEXR/ImathBox.h>

#include<ramen/anim/any_curve.hpp>

#include<ramen/ui/anim/anim_curves_view_fwd.hpp>

namespace ramen
{
namespace ui
{

// utility function. Also used outside of here, so it's public.
void tangents_mask( const anim::float_key_t& k, const anim::float_key_t *prev, const anim::float_key_t *next, bool& left, bool& right);

// visitors
struct draw_curve_visitor : public boost::static_visitor<>
{
    draw_curve_visitor( const anim_curves_view_t& view);
    void operator()( const anim::float_curve_t *c);
	void operator ()( const anim::shape_curve2f_t *c);

private:

    const anim_curves_view_t& view_;
};

struct draw_keyframes_visitor : public boost::static_visitor<>
{
    draw_keyframes_visitor( const anim_curves_view_t& view, bool draw_tangents = true);
    void operator()( const anim::float_curve_t *c);
	void operator ()( const anim::shape_curve2f_t *c);

private:

    const anim_curves_view_t& view_;
	bool draw_tangents_;
};

struct bbox_curve_visitor : public boost::static_visitor<>
{
    bbox_curve_visitor( const Imath::Box2f box = Imath::Box2f(), bool sel_only = false);
    void operator()( const anim::float_curve_t *c);
	void operator ()( const anim::shape_curve2f_t *c);
	
    Imath::Box2f bbox;
    bool selected_only;
};

} // namespace
} // namespace

#endif
