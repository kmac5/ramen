// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_SYSTEM_ENUMS_HPP
#define RAMEN_SYSTEM_ENUMS_HPP

namespace ramen
{
namespace system
{

enum cpu_family_type
{
    cpu_unknown = 0,
    cpu_x86
};

enum simd_instruction_set
{
    simd_none = 0,

    // intel
    simd_sse	= 1 << 0,
    simd_sse2	= 1 << 1,
    simd_sse3	= 1 << 2,
};

} // namespace
} // namespace

#endif
