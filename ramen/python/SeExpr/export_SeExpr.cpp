// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/python/python.hpp>

#include<ramen/python/SeExpr/export_SeExpr.hpp>

#include<boost/python.hpp>
#include<boost/python/suite/indexing/vector_indexing_suite.hpp>
#include<boost/python/suite/indexing/map_indexing_suite.hpp>
namespace bpy = boost::python;

#include<SeExpression.h>

bool operator==( const SeExpression::Error& a, const SeExpression::Error& b)
{
	return a.error == b.error && a.startPos == b.startPos && a.endPos == b.endPos;
}

bool operator!=( const SeExpression::Error& a, const SeExpression::Error& b)
{
	return a.error != b.error || a.startPos != b.startPos || a.endPos != b.endPos;
}

namespace ramen
{
namespace python
{
namespace
{

bpy::tuple get_value( const SeExprLocalVarRef& var)
{
	return bpy::make_tuple( var.val[0], var.val[1], var.val[2]);
}

bpy::tuple evaluate( const SeExpression& expr)
{
	SeVec3d val( expr.evaluate());
	return bpy::make_tuple( val[0], val[1], val[2]);
}

} // unnamed

void export_SeExpr()
{
	bpy::object SeExpt_module( bpy::handle<>( bpy::borrowed( PyImport_AddModule( "_ramen.SeExpr"))));
	bpy::scope().attr( "SeExpr") = SeExpt_module;
	bpy::scope SeExpr_module_scope = SeExpt_module;

	bpy::class_<SeExpression::Error>( "Error", bpy::init<std::string, int, int>())
			.def_readwrite( "error", &SeExpression::Error::error)
			.def_readwrite( "startPos", &SeExpression::Error::startPos)
			.def_readwrite( "endPos", &SeExpression::Error::endPos)
			;

	typedef std::vector<SeExpression::Error> ErrorList;
	bpy::class_<ErrorList>( "ErrorList")
			.def( bpy::vector_indexing_suite<ErrorList>())
			;

	bpy::class_<SeExprLocalVarRef>( "SeExprLocalVarRef")
			.def( "setIsVec", &SeExprLocalVarRef::setIsVec)
			.def( "isVec", &SeExprLocalVarRef::isVec)
			.add_property( "val", &get_value)
			;

	bpy::class_<SeExpression::LocalVarTable>( "LocalVarTable")
	        .def( bpy::map_indexing_suite<SeExpression::LocalVarTable>())
			;

	bpy::class_<SeExpression, boost::noncopyable>( "SeExpression")
			.def( bpy::init<std::string, bpy::optional<bool> >())
			.def( "setWantVec", &SeExpression::setWantVec)
			.def( "setExpr", &SeExpression::setExpr)
			.def( "getExpr", &SeExpression::getExpr, bpy::return_value_policy<bpy::copy_const_reference>())
			.def( "syntaxOK", &SeExpression::syntaxOK)
			.def( "isValid", &SeExpression::isValid)
			.def( "parseError", &SeExpression::parseError, bpy::return_value_policy<bpy::copy_const_reference>())
			.def( "getErrors", &SeExpression::getErrors, bpy::return_internal_reference<>())
			.def( "isConstant", &SeExpression::isConstant)
			.def( "usesVar", &SeExpression::usesVar)
			.def( "usesFunc", &SeExpression::usesFunc)
			.def( "isThreadSafe", &SeExpression::isThreadSafe)
			.def( "wantVec", &SeExpression::wantVec)
			.def( "isVec", &SeExpression::isVec)
			.def( "reset", &SeExpression::reset)
			.def( "evaluate", &evaluate)
			.def( "getLocalVars", &SeExpression::getLocalVars, bpy::return_internal_reference<>())
			;
}

} // python
} // ramen
