// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/python/python.hpp>

#include<boost/python.hpp>
namespace bpy = boost::python;

#include<ramen/GL/gl.hpp>

namespace ramen
{
namespace python
{
namespace
{

} // unnamed

void export_gl()
{
	bpy::object gl_module( bpy::handle<>( bpy::borrowed( PyImport_AddModule( "_ramen._GL"))));
	bpy::scope().attr( "_GL") = gl_module;
	bpy::scope gl_scope = gl_module;

	// todo export all here
	//bpy::def( "gl_begin", gl_begin);
	//bpy::def( "gl_end", gl_end);
}
	
} // python
} // ramen
