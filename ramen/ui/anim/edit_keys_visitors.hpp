// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_ANIM_EDIT_KEYS_VISITORS_HPP
#define	RAMEN_UI_ANIM_EDIT_KEYS_VISITORS_HPP

#include<vector>
#include<map>

#include<boost/optional.hpp>

#include<OpenEXR/ImathVec.h>

#include<ramen/assert.hpp>

#include<ramen/anim/track_fwd.hpp>
#include<ramen/anim/any_curve.hpp>
#include<ramen/anim/curve.hpp>
#include<ramen/anim/curve_algorithm.hpp>

#include<ramen/ui/anim/anim_editor_command_fwd.hpp>
#include<ramen/ui/anim/anim_curves_view_fwd.hpp>

namespace ramen
{
namespace ui
{

struct delete_selected_keys_visitor : public boost::static_visitor<>
{
    delete_selected_keys_visitor() {}
	
	template<class K>
	void operator()( anim::curve_t<K> *c)
	{
		c->erase_selected_keyframes();
	}

    void operator()( anim::float_curve_t *c);
};

struct can_drag_keys_visitor : public boost::static_visitor<>
{	
	can_drag_keys_visitor( const Imath::V2f& offset);
	void operator()( const anim::float_curve_t *c);
	void operator()( const anim::shape_curve2f_t *c);
	bool result;

private:

	Imath::V2f offset_;	
};

struct drag_keys_visitor : public boost::static_visitor<>
{
	drag_keys_visitor( const Imath::V2f& offset, bool snap_frames);
	void operator()( anim::float_curve_t *c);
	void operator()( anim::shape_curve2f_t *c);

	bool changed;

private:

	Imath::V2f offset_;	
	bool snap_frames_;
};

struct get_keyframe_time_visitor : public boost::static_visitor<>
{
	get_keyframe_time_visitor( int index) : index_( index) {}
	
	template<class K>
	void operator()( const anim::curve_t<K> *c) { time = (*c)[index_].time();}

	float time;

private:

	int index_;
};

struct get_keyframe_value_visitor : public boost::static_visitor<>
{
	get_keyframe_value_visitor( int index);
	void operator()( const anim::float_curve_t *c);
	void operator()( const anim::shape_curve2f_t *c)
	{ 
		value = index_;
		can_edit = false;
	}
	
	float value;
	bool can_edit;

private:

	int index_;
};

struct can_insert_keyframe_visitor : public boost::static_visitor<>
{
	can_insert_keyframe_visitor( const anim_curves_view_t& view, const Imath::V2i& p, bool snap);
	
	template<class K>
	void operator()( const anim::curve_t<K> *c) { can_insert = false;}

	void operator()( const anim::float_curve_t *c);

	bool can_insert;
	
private:

	const anim_curves_view_t& view_;
	Imath::V2i p_;
	float time_;
	bool snap_;
};

struct insert_keyframe_visitor : public boost::static_visitor<>
{
	insert_keyframe_visitor( const anim_curves_view_t& view, const Imath::V2i& p, bool snap);

	template<class K>
	void operator()( anim::curve_t<K> *c) { RAMEN_ASSERT( 0);}

	void operator()( anim::float_curve_t *c);
	
	int key_index;
	
private:

	const anim_curves_view_t& view_;
	Imath::V2i p_;
	float time_;
	bool snap_;
};

struct reverse_selected_keys_visitor : public boost::static_visitor<>
{
    reverse_selected_keys_visitor() {}
	
	template<class K>
	void operator()( anim::curve_t<K> *c)		{ anim::reverse_keyframes( *c, true);}
	
	void operator()( anim::float_curve_t *c)	{ anim::reverse_float_keyframes( *c, true);}
};

struct negate_selected_keys_visitor : public boost::static_visitor<>
{
    negate_selected_keys_visitor() {}
	
	template<class K>
	void operator()( anim::curve_t<K> *c) { RAMEN_ASSERT( 0);}

    void operator()( anim::float_curve_t *c);
};

struct sample_selected_keys_visitor : public boost::static_visitor<>
{
    sample_selected_keys_visitor() {}
	
	template<class K>
	void operator()( anim::curve_t<K> *c) { RAMEN_ASSERT( 0);}

    void operator()( anim::float_curve_t *c);
};

struct smooth_selected_keys_visitor : public boost::static_visitor<>
{
    smooth_selected_keys_visitor( float filter_size, bool resample) : filter_size_( filter_size), resample_( resample) {}
	
	template<class K>
	void operator()( anim::curve_t<K> *c) { RAMEN_ASSERT( 0);}

    void operator()( anim::float_curve_t *c);
	
private:
	
	float filter_size_;
	bool resample_;
};

struct high_pass_selected_keys_visitor : public boost::static_visitor<>
{
    high_pass_selected_keys_visitor( float filter_size, bool resample) : filter_size_( filter_size), resample_( resample) {}
	
	template<class K>
	void operator()( anim::curve_t<K> *c) { RAMEN_ASSERT( 0);}

    void operator()( anim::float_curve_t *c);
	
private:
	
	float filter_size_;
	bool resample_;
};

} // namespace
} // namespace

#endif
