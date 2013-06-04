// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_EXPRESSIONS_SE_EXPRESSION_HPP
#define RAMEN_EXPRESSIONS_SE_EXPRESSION_HPP

#include<SeExpression.h>
#include<SeExprFunc.h>

#include<boost/ptr_container/ptr_vector.hpp>

namespace ramen
{
namespace expressions
{
	
class se_expression_t : public SeExpression
{
public:
	
	se_expression_t();
	se_expression_t( const std::string &e, bool wantVec = 1);

	virtual void reset();

	virtual const std::string& getExpr() const;
	virtual void setExpr(const std::string& e);

	virtual bool syntaxOK() const;
	virtual bool isValid() const;

	virtual bool usesVar(const std::string& name) const;
	virtual bool usesFunc(const std::string& name) const;

	virtual SeExprFunc *resolveFunc( const std::string& name) const;

protected:

	mutable boost::ptr_vector<SeExprFunc> funcs_;
};
	
} // ramen
} // expression

#endif
