// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_PYTHON_UTIL
#define RAMEN_PYTHON_UTIL

#include<ramen/python/python.hpp>

#include<boost/python.hpp>

#include<OpenEXR/ImathVec.h>
#include<OpenEXR/ImathBox.h>
#include<OpenEXR/ImathColor.h>

namespace ramen
{
namespace python
{
namespace detail
{

template<class T>
void check_list_and_set_py_err( const boost::python::list& t, int n)
{
	if( boost::python::len( t) != n)
    {
		//PyErr_SetObject( PyExc_ValueError, "expected tuple with 2 elements");
		boost::python::throw_error_already_set();
    }
}

template<class T>
boost::python::list make_list( T x, T y)
{
	boost::python::list result;
	result.append( x);
	result.append( y);
	return result;
}

template<class T>
boost::python::list make_list( T x, T y, T z)
{
	boost::python::list result;
	result.append( x);
	result.append( y);
	result.append( z);
	return result;
}

template<class T>
boost::python::list make_list( T x, T y, T z, T w)
{
	boost::python::list result;
	result.append( x);
	result.append( y);
	result.append( z);
	result.append( w);
	return result;
}

} // detail

template<class T>
boost::python::list vec_to_list( const Imath::Vec2<T>& v)
{
	return detail::make_list( v.x, v.y);
}

template<class T>
boost::python::list vec_to_list( const Imath::Vec3<T>& v)
{
	return detail::make_list( v.x, v.y, v.z);
}

template<class T>
boost::python::list color_to_list( const Imath::Color3<T>& v)
{
	return detail::make_list( v.x, v.y, v.z);
}

template<class T>
boost::python::list color_to_list( const Imath::Color4<T>& v)
{
	return detail::make_list( v.r, v.g, v.b, v.a);
}

template<class V>
boost::python::list box_to_list( const Imath::Box<V>& box)
{
	return detail::make_list( box.min.x, box.min.y, box.max.x, box.max.y);
}

template<class T>
Imath::Vec2<T> list_to_vec2( const boost::python::list& t)
{
	detail::check_list_and_set_py_err<T>( t, 2);

	Imath::Vec2<T> v;
	v.x = boost::python::extract<T>( t[0]);
	v.y = boost::python::extract<T>( t[1]);
	return v;
}

template<class T>
Imath::Vec3<T> list_to_vec3( const boost::python::list& t)
{
	detail::check_list_and_set_py_err<T>( t, 3);

	Imath::Vec3<T> v;
	v.x = boost::python::extract<T>( t[0]);
	v.y = boost::python::extract<T>( t[1]);
	v.z = boost::python::extract<T>( t[2]);
	return v;
}

template<class T>
Imath::Color3<T> list_to_color3( const boost::python::list& t)
{
	detail::check_list_and_set_py_err<T>( t, 3);

	Imath::Color3<T> v;
	v.x = boost::python::extract<T>( t[0]);
	v.y = boost::python::extract<T>( t[1]);
	v.z = boost::python::extract<T>( t[2]);
	return v;
}

template<class T>
Imath::Color4<T> list_to_color4( const boost::python::list& t)
{
	detail::check_list_and_set_py_err<T>( t, 4);

	Imath::Color4<T> v;
	v.r = boost::python::extract<T>( t[0]);
	v.g = boost::python::extract<T>( t[1]);
	v.b = boost::python::extract<T>( t[2]);
	v.a = boost::python::extract<T>( t[3]);
	return v;
}

} // python
} // ramen

#endif
