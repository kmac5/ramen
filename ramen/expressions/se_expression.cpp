// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/python/python.hpp>

#include<ramen/expressions/se_expression.hpp>

namespace ramen
{
namespace expressions
{

se_expression_t::se_expression_t() : SeExpression() {}
se_expression_t::se_expression_t( const std::string& e, bool wantVec) : SeExpression( e, wantVec) {}

void se_expression_t::reset()
{
	funcs_.clear();
	SeExpression::reset();
}

const std::string& se_expression_t::getExpr() const
{
	return SeExpression::getExpr();
}

void se_expression_t::setExpr(const std::string& e)
{
	reset();
	SeExpression::setExpr( e);
}

bool se_expression_t::syntaxOK() const
{
	bool result = SeExpression::syntaxOK();
	return result;
}

bool se_expression_t::isValid() const
{
	bool result = SeExpression::isValid();
	return result;
}

bool se_expression_t::usesVar(const std::string& name) const
{
	bool result = SeExpression::usesVar( name);
	return result;
}

bool se_expression_t::usesFunc(const std::string& name) const
{
	bool result = SeExpression::usesFunc( name);
	return result;
}

SeExprFunc *se_expression_t::resolveFunc( const std::string& name) const { return 0;}

} // ramen
} // expressions
