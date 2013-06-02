// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/ui/expression_editor.hpp>

#include<ramen/app/application.hpp>

#include<ramen/python/interpreter.hpp>
#include<ramen/python/pyside.hpp>

#include<ramen/ui/user_interface.hpp>

namespace ramen
{
namespace ui
{

expression_editor_t& expression_editor_t::instance()
{
    static expression_editor_t expr;
    return expr;
}

expression_editor_t::expression_editor_t()
{
	try
	{
		create_expr_editor_ = python::interpreter_t::instance().eval( "ramen.ui.create_expression_editor");
	}
	catch( boost::python::error_already_set)
	{
		PyErr_Print();
		PyErr_Clear();
	}
}

void expression_editor_t::show( expressions::se_expression_t& expr, const expression_editor_callback_suite_t& callbacks)
{
	if( create_expr_editor_ != boost::python::object())
	{
		try
		{
			create_expr_editor_( boost::python::ptr( &expr), boost::python::ptr( &callbacks),
								 python::make_pyside_wrapper( reinterpret_cast<QMainWindow*>( app().ui()->main_window())));
		}
		catch( boost::python::error_already_set)
		{
			PyErr_Print();
			PyErr_Clear();
		}
	}
}

} // namespace
} // namespace
