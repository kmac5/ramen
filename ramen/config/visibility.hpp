// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_VISIBILITY_HPP
#define RAMEN_VISIBILITY_HPP

#include<ramen/config/compiler.hpp>

#if defined( RAMEN_CONFIG_COMPILER_GNUC)
	#define RAMEN_EXPORT  __attribute__ (( visibility( "default")))
	#define RAMEN_IMPORT  __attribute__ (( visibility( "default")))
#elif defined( RAMEN_CONFIG_COMPILER_MSVC)
    #define RAMEN_EXPORT  __declspec( dllexport)
    #define RAMEN_IMPORT  __declspec( dllimport)
#else
	#define RAMEN_EXPORT
	#define RAMEN_IMPORT
#endif

#endif
