// Copyright (c) 2010 Esteban Tovagliari

#include<boost/python.hpp>
namespace bpy = boost::python;

#include<boost/intrusive_ptr.hpp>

#include<ramen/ref_ptr.hpp>

#include<ramen/python/SeExpr/export_SeExpr.hpp>

namespace boost
{
namespace python
{

template <class T>
struct pointee<intrusive_ptr<T> >
{
	typedef T type;
};

template <class T>
T* get_pointer( intrusive_ptr<T> const& p) { return p.get();}

// ref_ptr_t
template <class T>
struct pointee<ramen::ref_ptr_t<T> >
{
	typedef T type;
};

template <class T>
T* get_pointer( ramen::ref_ptr_t<T> const& p) { return p.get();}

} // python
} // boost

namespace ramen
{
namespace python
{

// prototypes
void export_anim();
void export_composition();
void export_expressions();
void export_gl();
void export_image();
void export_manipulable();
void export_manipulator();
void export_node();
void export_param();
void export_parameterised();
void export_python();
void export_roto();
void export_ui();

void export_ramen()
{
	bpy::object package = bpy::scope();
	package.attr("__path__") = "_ramen";
	
	export_param();
	export_parameterised();
	export_manipulable();
	export_manipulator();
	export_node();
	export_composition();
	export_anim();
	export_roto();
	export_ui();
	export_image();
	export_python();

	// util modules
	export_gl();

	export_SeExpr();
	export_expressions();
}

} // python
} // ramen
