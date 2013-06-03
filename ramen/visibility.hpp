// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_VISIBILITY_HPP
#define RAMEN_VISIBILITY_HPP

// visibility
#if defined( __GNUC__)
	#define RAMEN_EXPORT  __attribute__ (( visibility( "default")))
	#define RAMEN_IMPORT  __attribute__ (( visibility( "default")))
	#define RAMEN_PRIVATE __attribute__ (( visibility( "hidden")))
#else
	#define RAMEN_EXPORT
	#define RAMEN_IMPORT
    #define RAMEN_PRIVATE
#endif

#ifdef ramen_EXPORTS
    #define RAMEN_API RAMEN_EXPORT
#else
    #define RAMEN_API RAMEN_IMPORT
#endif

#endif
