// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_PYTHON_PYSIDE_HPP
#define RAMEN_PYTHON_PYSIDE_HPP

#include<ramen/python/python.hpp>

#include<boost/python/object.hpp>

#include<PySide/QtGui/pyside_qtgui_python.h>

class QWidget;

namespace ramen
{
namespace python
{

void init_pyside();

template<class T>
T *pyside_to_cpp( PyObject *o) { return Shiboken::Converter<T*>::toCpp( o);}

template<class T>
T *pyside_to_cpp( const boost::python::object& o) { return pyside_to_cpp<T>( o.ptr());}

template<class T>
boost::python::object pyside_to_python( T *o)
{
	PyObject *p = Shiboken::Converter<T*>::toPython( o);
	return boost::python::object( boost::python::handle<>( p));
}

template<class T>
boost::python::object make_pyside_wrapper( T *o, bool has_ownership = false, bool exact_type = true)
{
	PyObject *p = Shiboken::createWrapper<T>( o, has_ownership, exact_type);
	return boost::python::object( boost::python::handle<>( p));
}

} // python
} // ramen

#endif
