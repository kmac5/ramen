// Copyright (c) 2011 Esteban Tovagliari

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

#ifdef RAMEN_MAKING_DSO
    #define RAMEN_API RAMEN_EXPORT
#else
    #define RAMEN_API RAMEN_IMPORT
#endif

#endif
