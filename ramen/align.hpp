// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_ALIGN_HPP
#define RAMEN_ALIGN_HPP

#ifdef __CUDACC__
	#define RAMEN_ALIGN(x)  __align__(x)
#else
	#if defined(__GNUC__)
		#define RAMEN_ALIGN(x)  __attribute__ ((aligned (x)))
		//#define RAMEN_PACKED    __attribute__ ((__packed__))
	#else
		#if defined(_MSC_VER)
			#define RAMEN_ALIGN(x) __declspec(align(x))
		#else
			// all other compilers
			#define RAMEN_ALIGN(x)
		#endif
	#endif
#endif

#endif
