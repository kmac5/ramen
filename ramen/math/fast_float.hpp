//////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Image Engine Design Inc. All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are
//  met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//
//     * Neither the name of Image Engine Design nor the names of any
//       other contributors to this software may be used to endorse or
//       promote products derived from this software without specific prior
//       written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
//  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
//  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//////////////////////////////////////////////////////////////////////////

#ifndef RAMEN_MATH_FAST_FLOAT_HPP
#define RAMEN_MATH_FAST_FLOAT_HPP

#include<ramen/config.hpp>

namespace ramen
{
namespace math
{

#define RAMEN_DOUBLEMAGICROUNDEPS ( .5-1.4e-11)
#define RAMEN_DOUBLEMAGIC double ( 6755399441055744.0)

union fast_float_double_long
{
	double d;
	long l;
};

inline int fast_float_round( double v)
{
	fast_float_double_long vv;
    vv.d = v + RAMEN_DOUBLEMAGIC;
	return vv.l;
}

inline int fast_float_to_int( double v)
{
    return ( v < 0.0) ? fast_float_round( v + RAMEN_DOUBLEMAGICROUNDEPS) : fast_float_round( v - RAMEN_DOUBLEMAGICROUNDEPS);
}

inline int fast_float_floor( double v)
{
    return fast_float_round( v - RAMEN_DOUBLEMAGICROUNDEPS);
}

inline int fast_float_ceil( double v)
{
    return fast_float_round( v + RAMEN_DOUBLEMAGICROUNDEPS);
}

} // math
} // ramen

#endif

