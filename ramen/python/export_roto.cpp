// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/python/python.hpp>

#include<boost/python.hpp>
namespace bpy = boost::python;

#include<ramen/nodes/image/roto/roto_node.hpp>

#include<ramen/app/composition.hpp>

namespace ramen
{
namespace python
{

void export_roto()
{
	bpy::object roto_module( bpy::handle<>( bpy::borrowed( PyImport_AddModule( "_ramen._roto"))));
	bpy::scope().attr( "_roto") = roto_module;
	bpy::scope roto_scope = roto_module;

    /*
	bpy::class_<roto::shape_t, roto::shape_ptr_t>( "shape_t")
		;
	
	bpy::class_<image::roto_node_t, bpy::bases<image_node_t>, image::roto_node_ptr_t, boost::noncopyable>( "roto_node_t", bpy::no_init)
		;
        */
}

} // python
} // ramen
