// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/python/python.hpp>

#include<boost/python.hpp>
namespace bpy = boost::python;

#include<ramen/image/buffer.hpp>

#include<ramen/python/util.hpp>

namespace ramen
{
namespace python
{
namespace
{

/*
template<class View>
void export_gil_view( const char *name)
{
	typedef View view_type;

	bpy::class_<view_type>( name)
		.def( "width", &view_type::width)
		.def( "height", &view_type::height)
		.def( "num_channels", &view_type::num_channels)
		;
}
*/

} // unnamed

void export_image()
{
	bpy::object image_module( bpy::handle<>( bpy::borrowed( PyImport_AddModule( "_ramen._image"))));
	bpy::scope().attr( "_image") = image_module;
	bpy::scope image_scope = image_module;

    /*
	export_gil_view<image::const_image_view_t>( "const_image_view_t");
	export_gil_view<image::image_view_t>( "image_view_t");

	export_gil_view<image::const_rgb_image_view_t>( "const_rgb_image_view_t");
	export_gil_view<image::rgb_image_view_t>( "rgb_image_view_t");

	export_gil_view<image::const_gray_image_view_t>( "const_gray_image_view_t");
	export_gil_view<image::gray_image_view_t>( "gray_image_view_t");

	bpy::class_<image::buffer_t>( "buffer_t")
		.def( "width", &image::buffer_t::width)
		.def( "height", &image::buffer_t::height)
		.def( "channels", &image::buffer_t::channels)
		.def( "empty", &image::buffer_t::empty)
		.def( "clear", &image::buffer_t::clear)

		.def( "const_rgba_view", &image::buffer_t::const_rgba_view)
		.def( "const_rgb_view", &image::buffer_t::const_rgb_view)
		.def( "const_gray_view", &image::buffer_t::const_gray_view)

		.def( "rgba_view", &image::buffer_t::rgba_view)
		.def( "rgb_view", &image::buffer_t::rgb_view)
		.def( "gray_view", &image::buffer_t::gray_view)
		;
    */
}

} // python
} // ramen
