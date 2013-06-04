// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/python/python.hpp>

#include<boost/python.hpp>
namespace bpy = boost::python;

#include<ramen/params/param.hpp>
#include<ramen/params/numeric_param.hpp>

#include<ramen/python/access.hpp>

namespace ramen
{
namespace python
{

namespace
{
/*
bpy::object get_param_value( const param_t& p)
{
	access a;
	return a.get_param_value( p);
}

bpy::object get_param_value_at_frame( const param_t& p, float frame)
{
	access a;
	return a.get_param_value( p, frame);
}
*/
} // unnamed
	
void export_param()
{
    /*
	bpy::enum_<param_t::change_reason>( "param_change_reason")
			.value( "silent_edit", param_t::silent_edit)
			.value( "user_edited", param_t::user_edited)
			.value( "node_edited", param_t::node_edited)
			.value( "node_loaded", param_t::node_loaded)
			.value( "time_changed", param_t::time_changed)
			;

	bpy::class_<param_t, boost::noncopyable, std::auto_ptr<param_t> >( "param_t", bpy::no_init)
			.def( "name", &param_t::name, bpy::return_value_policy<bpy::copy_const_reference>())
			.def( "id", &param_t::id, bpy::return_value_policy<bpy::copy_const_reference>())
			
			.def( "value", get_param_value)
			.def( "value_at_frame", get_param_value_at_frame)
			;
            */
}
	
} // python
} // ramen
