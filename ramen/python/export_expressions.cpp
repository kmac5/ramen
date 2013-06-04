// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/python/python.hpp>

#include<boost/python.hpp>
namespace bpy = boost::python;

#include<ramen/expressions/se_expression.hpp>

namespace ramen
{
namespace python
{

void export_expressions()
{
	bpy::object expressions_module( bpy::handle<>( bpy::borrowed( PyImport_AddModule( "_ramen._expressions"))));
	bpy::scope().attr( "_expressions") = expressions_module;
	bpy::scope expressions_scope = expressions_module;

	bpy::class_<expressions::se_expression_t, bpy::bases<SeExpression>, boost::noncopyable>( "se_expression_t", bpy::no_init)
		.def( "setExpr", &expressions::se_expression_t::setExpr)
		.def( "getExpr", &expressions::se_expression_t::getExpr, bpy::return_value_policy<bpy::copy_const_reference>())
		.def( "syntaxOK", &expressions::se_expression_t::syntaxOK)
		.def( "isValid", &expressions::se_expression_t::isValid)
		.def( "usesVar", &expressions::se_expression_t::usesVar)
		.def( "usesFunc", &expressions::se_expression_t::usesFunc)
		;
}

} // python
} // ramen
