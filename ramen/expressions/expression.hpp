// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_EXPRESSIONS_EXPRESSION_HPP
#define RAMEN_EXPRESSIONS_EXPRESSION_HPP

#include<string>

#include<ramen/nodes/node_fwd.hpp>

namespace ramen
{
namespace expressions
{
	
class expression_t
{
public:
	
	expression_t();
	~expression_t();

private:

	struct impl;
	impl *pimpl_;
};
	
} // ramen
} // expression

#endif
