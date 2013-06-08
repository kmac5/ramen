// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_MEMORY_UTIL_HPP
#define RAMEN_MEMORY_UTIL_HPP

#include<stdint.h>

#include<ramen/assert.hpp>

namespace ramen
{
namespace memory
{

template<class T>
T *aligned_ptr( T *p, int alignment)
{
	RAMEN_ASSERT( (( alignment - 1) & alignment) == 0);

    uintptr_t ptr = reinterpret_cast<uintptr_t>( p);
    uintptr_t align = alignment - 1;
    uintptr_t aligned = ( ptr + align + 1) & ~align;

    return reinterpret_cast<unsigned char *>( aligned);
}

} // namespace
} // namespace

#endif
