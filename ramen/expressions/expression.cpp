// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/python/python.hpp>

#include<ramen/expressions/expression.hpp>

#include<boost/noncopyable.hpp>

#include<ramen/assert.hpp>

namespace ramen
{
namespace expressions
{

struct expression_t::impl : boost::noncopyable {};

expression_t::expression_t() : pimpl_( 0) {}
expression_t::~expression_t() {}

} // ramen
} // expressions
