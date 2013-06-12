// Copyright (c) 2013 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/core/empty.hpp>

namespace ramen
{
namespace core
{

bool empty_t::operator==( const empty_t&) const  { return true;}
bool empty_t::operator<( const empty_t&) const   { return false;}

void empty_t::swap( empty_t&) {}

} // core
} // ramen
