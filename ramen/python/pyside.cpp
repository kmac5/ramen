// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/python/pyside.hpp>

#include<shiboken/sbkmodule.h>
#include<PySide/QtGui/pyside_qtgui_python.h>

PyObject *g_qtcore_module = 0;
PyObject *g_qtgui_module = 0;
PyObject *g_qtgl_module = 0;

PyTypeObject **SbkPySide_QtCoreTypes;
PyTypeObject **SbkPySide_QtGuiTypes;
PyTypeObject **SbkPySide_QtOpenGLTypes;

namespace ramen
{
namespace python
{

void init_pyside()
{
    g_qtcore_module = Shiboken::Module::import( "PySide.QtCore");

    if( !g_qtcore_module)
        PyErr_SetString( PyExc_ImportError, "could not import PySide.QtCore");

    SbkPySide_QtCoreTypes = Shiboken::Module::getTypes( g_qtcore_module);

    g_qtgui_module = Shiboken::Module::import( "PySide.QtGui");

    if( !g_qtgui_module)
        PyErr_SetString( PyExc_ImportError, "could not import PySide.QtGui");

    SbkPySide_QtGuiTypes = Shiboken::Module::getTypes( g_qtgui_module);

    g_qtgl_module = Shiboken::Module::import( "PySide.QtOpenGL");

    if( !g_qtgl_module)
        PyErr_SetString( PyExc_ImportError, "could not import PySide.QtOpenGL");

    SbkPySide_QtOpenGLTypes = Shiboken::Module::getTypes( g_qtgl_module);
}

} // python
} // ramen
