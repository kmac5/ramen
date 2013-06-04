// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/python/python.hpp>

#include<boost/python.hpp>
namespace bpy = boost::python;

#include<ramen/python/interpreter.hpp>

namespace ramen
{
namespace python
{
namespace
{
/*
class py_interpreter_t
{
public:

	py_interpreter_t() {}

    boost::python::object exec( const std::string& cmd)
	{
		return python::interpreter_t::Instance().exec( cmd);
	}

    boost::python::object exec2( const std::string& cmd, boost::python::object global, boost::python::object local)
	{
		return python::interpreter_t::Instance().exec( cmd, global, local);
	}

	boost::python::object eval( const std::string& expr)
	{
		return python::interpreter_t::Instance().eval( expr);
	}

    boost::python::object eval2( const std::string& expr, boost::python::object global, boost::python::object local)
	{
		return python::interpreter_t::Instance().eval( expr, global, local);
	}

	static py_interpreter_t get_interpreter()
	{
		return py_interpreter_t();
	}
};
*/
} // unnamed

void export_python()
{
	bpy::object python_module( bpy::handle<>( bpy::borrowed( PyImport_AddModule( "_ramen._python"))));
	bpy::scope().attr( "_python") = python_module;
	bpy::scope python_scope = python_module;

    /*
	bpy::class_<py_interpreter_t>( "interpreter_t", bpy::no_init)
		.def( "exec", &py_interpreter_t::exec)
		.def( "exec", &py_interpreter_t::exec2)
		.def( "eval", &py_interpreter_t::eval)
		.def( "eval", &py_interpreter_t::eval2)
		;

	bpy::def( "interpreter", &py_interpreter_t::get_interpreter);
    */
}

} // python
} // ramen
