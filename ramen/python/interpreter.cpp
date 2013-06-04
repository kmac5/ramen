// Copyright (c) 2010 Esteban Tovagliari

#include"Python.h"

#include<ramen/python/interpreter.hpp>

#include<boost/filesystem/operations.hpp>

#include<ramen/app/application.hpp>

#include<ramen/filesystem/path.hpp>

#include<ramen/python/export_ramen.hpp>

#include<ramen/python/pyside.hpp>

#include<iostream>

namespace bpy = boost::python;
namespace bfs = boost::filesystem;

// main python module
BOOST_PYTHON_MODULE( _ramen)
{
    ramen::python::export_ramen();
}

namespace ramen
{
namespace python
{

interpreter_t& interpreter_t::instance()
{
    static interpreter_t i;
    return i;
}

interpreter_t::interpreter_t()
{
    PyImport_AppendInittab(( char *) "_ramen", &init_ramen);
    Py_Initialize();

    bpy::object main_module( bpy::handle<>( bpy::borrowed( PyImport_AddModule( "__main__"))));
    main_namespace_ = main_module.attr( "__dict__");

	setup_python_paths( main_namespace_);

	init_pyside();

    // default import our module.
    bpy::object module(( bpy::handle<>( PyImport_ImportModule( "ramen"))));
    main_namespace_[ "ramen"] = module;
    exec( "from ramen import *");

    // execute startup files
    bfs::path exec_path = app().system().executable_path();
    bfs::path py_path = exec_path.parent_path() / "../python";
    exec_file( py_path / "app/init.py");

    exec_file( app().system().home_path() / "ramen/python/init.py");
}

bpy::object interpreter_t::exec( const std::string& cmd)
{
    return bpy::exec( bpy::str( cmd.c_str()), main_namespace_, main_namespace_);
}

boost::python::object interpreter_t::exec( const std::string& cmd, boost::python::object global, boost::python::object local)
{
    return bpy::exec( bpy::str( cmd.c_str()), global, local);
}

bpy::object interpreter_t::eval( const std::string& expr)
{
    return bpy::eval( bpy::str( expr.c_str()), main_namespace_, main_namespace_);
}

boost::python::object interpreter_t::eval( const std::string& expr, boost::python::object global, boost::python::object local)
{
    return bpy::eval( bpy::str( expr.c_str()), global, local);
}

bpy::object interpreter_t::exec_file( const boost::filesystem::path& p)
{
    if( boost::filesystem::exists( p))
		return bpy::exec_file( bpy::str( filesystem::file_cstring( p)), main_namespace_, main_namespace_);

    return bpy::object();
}

void interpreter_t::setup_python_paths( boost::python::object name_space)
{
    bfs::path exec_path = app().system().executable_path();
    bfs::path py_path = exec_path.parent_path() / "../python";

    // add our py app dir to python's search path
	std::stringstream cmd;
	cmd << "import sys\n";
	add_path_to_sys_cmd( cmd, py_path / "app");
    exec( cmd.str(), name_space, name_space);
}

void interpreter_t::add_path_to_sys_cmd( std::stringstream& cmd, const boost::filesystem::path& p) const
{
	cmd << "sys.path.append( '" << filesystem::file_string( p) << "')\n";
}

} // namespace
} // namespace
