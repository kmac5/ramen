// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/python/python.hpp>

#include<boost/python.hpp>
namespace bpy = boost::python;

#include<ramen/manipulators/manipulable.hpp>

namespace ramen
{
namespace python
{

void export_manipulable()
{
	//bpy::class_<manipulable_t, bpy::bases<parameterised_t>, boost::noncopyable, manipulable_ptr_t>( "manipulable_t", bpy::no_init)
	//	;
}
	
} // python
} // ramen
