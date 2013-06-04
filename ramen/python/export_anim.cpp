// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/python/python.hpp>

#include<boost/python.hpp>
namespace bpy = boost::python;

#include<ramen/anim/float_curve.hpp>

namespace ramen
{
namespace python
{
namespace
{

/*
int tangent_step_k()	{ return (int) anim::keyframe_t::tangent_step;}
int tangent_linear_k()	{ return (int) anim::keyframe_t::tangent_linear;}
int tangent_smooth_k()	{ return (int) anim::keyframe_t::tangent_smooth;}
int tangent_flat_k()	{ return (int) anim::keyframe_t::tangent_flat;}
int tangent_fixed_k()	{ return (int) anim::keyframe_t::tangent_fixed;}

int v0_auto_tangent( const anim::keyframe_t& k) { return (int) k.v0_auto_tangent();}
int v1_auto_tangent( const anim::keyframe_t& k) { return (int) k.v1_auto_tangent();}

float get_value( const anim::float_key_t& k) { return k.value();}
*/

} // unnamed

void export_anim()
{
	bpy::object anim_module( bpy::handle<>( bpy::borrowed( PyImport_AddModule( "_ramen._anim"))));
	bpy::scope().attr( "_anim") = anim_module;
	bpy::scope anim_scope = anim_module;

    /*
	bpy::class_<anim::keyframe_t>( "keyframe_t", bpy::no_init)
		.def( "time", &anim::keyframe_t::time)
		
		.def( "selected", &anim::keyframe_t::selected)
		
		.def( "v0_auto_tangent", v0_auto_tangent)
		.def( "v1_auto_tangent", v1_auto_tangent)
		
		.add_property( "tangent_step"	, tangent_step_k)
		.add_property( "tangent_linear"	, tangent_linear_k)
		.add_property( "tangent_smooth"	, tangent_smooth_k)
		.add_property( "tangent_flat"	, tangent_flat_k)
		.add_property( "tangent_fixed"	, tangent_fixed_k)
		;
	
	bpy::class_<anim::float_key_t, bpy::bases<anim::keyframe_t> >( "float_key_t")
		.def( bpy::init<float,float>())
		.def( "value", get_value)
		.def( "v0", &anim::float_key_t::v0)
		.def( "v1", &anim::float_key_t::v1)
		.def( "tangent_continuity", &anim::float_key_t::tangent_continuity)
		;
		
	bpy::class_<anim::float_curve_t>( "float_curve_t")
		.def( "empty", &anim::float_curve_t::empty)
		.def( "size", &anim::float_curve_t::size)
		.def( "clear", &anim::float_curve_t::clear)
		.def( "start_time", &anim::float_curve_t::start_time)
		.def( "end_time", &anim::float_curve_t::end_time)
		.def( "erase", &anim::float_curve_t::erase)
		.def( "erase_selected_keyframes", &anim::float_curve_t::erase_selected_keyframes)
		.def( "select_all_keyframes", &anim::float_curve_t::select_all_keyframes)
		.def( "any_keyframe_selected", &anim::float_curve_t::any_keyframe_selected)
		.def( "has_keyframe_at", &anim::float_curve_t::has_keyframe_at)
		;
     */
}

} // python
} // ramen
