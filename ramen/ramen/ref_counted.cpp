// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ref_counted.hpp>

namespace ramen
{

ref_counted_t::ref_counted_t()  { num_refs_ = 0;}
ref_counted_t::~ref_counted_t() {}

} // namespace
