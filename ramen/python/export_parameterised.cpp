// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/python/python.hpp>

#include<boost/python.hpp>
namespace bpy = boost::python;

#include<ramen/params/parameterised.hpp>

#include<ramen/app/composition.hpp>

#include<ramen/python/access.hpp>

namespace ramen
{
namespace python
{
namespace
{
/*
composition_t *get_composition( parameterised_t& p) { return p.composition();}
node_t *get_node( parameterised_t& p)				{ return p.node();}

boost::python::list get_param_ids( const parameterised_t& p)
{
	access acc;
	return acc.get_param_ids( p.param_set());
}

param_t *find_param( parameterised_t& p, const std::string& id) { return &( p.param( id));}
*/
} // unnamed

void export_parameterised()
{
    /*
	bpy::class_<parameterised_t, boost::noncopyable, parameterised_ptr_t>( "parameterised_t", bpy::no_init)
		.def( "name", &parameterised_t::name, bpy::return_value_policy<bpy::copy_const_reference>())
		.def( "set_name", &parameterised_t::set_name)
	
		.def( "autokey", &parameterised_t::autokey)

		.def( "composition", get_composition, bpy::return_value_policy<bpy::reference_existing_object>())
		.def( "node", get_node, bpy::return_value_policy<bpy::reference_existing_object>())
		
		.def( "param_ids", get_param_ids)
		.def( "param", find_param, bpy::return_value_policy<bpy::reference_existing_object>())
		
		.def( "set_frame", &parameterised_t::set_frame)
		
		.def( "update_widgets", &parameterised_t::update_widgets)
		;
        */
}
	
} // python
} // ramen
