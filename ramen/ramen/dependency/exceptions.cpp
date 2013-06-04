// Copyright (c) 2012 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/dependency/exceptions.hpp>

namespace ramen
{
namespace dependency
{

const char *cycle_error::what() const throw()
{
	return "Cycle in dependency graph";
}

} // namespace
} // namespace
