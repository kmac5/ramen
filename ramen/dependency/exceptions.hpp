// Copyright (c) 2012 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_DEPENDENCY_EXCEPTIONS_HPP
#define RAMEN_DEPENDENCY_EXCEPTIONS_HPP

#include<ramen/config.hpp>

#include<exception>

namespace ramen
{
namespace dependency
{

/*!
\ingroup depgraph
\brief exception thrown when there's a cycle in the dependency graph.
*/
class cycle_error : public std::exception
{
public:

	virtual const char *what() const throw();
};

} // namespace
} // namespace

#endif
