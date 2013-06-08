// Copyright (c) 2013 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_CONFIG_COMPILER_HPP
#define RAMEN_CONFIG_COMPILER_HPP

#if defined( __GNUC__)
    #define RAMEN_CONFIG_COMPILER_GNUC
#elif defined( __clang__)
    #define RAMEN_CONFIG_COMPILER_CLANG
#elif defined( _MSC_VER)
    #define RAMEN_CONFIG_COMPILER_MSVC
#elif defined( __CYGWIN__)
    #define RAMEN_CONFIG_COMPILER_CYGWIN
#endif

#endif
