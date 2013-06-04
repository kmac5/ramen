// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGEIO_MEMORY_HPP
#define RAMEN_IMAGEIO_MEMORY_HPP

#include<stdio.h>

#include<adobe/memory.hpp>

namespace adobe
{

template<>
struct delete_ptr_trait<FILE*>
{
    void operator()( FILE *x) const
    {
        if( x)
            fclose( x);
    }
};

} // namespace

#endif
