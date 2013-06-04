// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_CONFIG_HPP
#define RAMEN_CONFIG_HPP

#include<ramen/visibility.hpp>

#ifndef NDEBUG
	#if defined(__GNUC__) && __GNUC__ > 3
		#define RAMEN_FORCEINLINE inline __attribute__ ((always_inline))
	#else
		#define RAMEN_FORCEINLINE inline
	#endif
#else
	#define RAMEN_FORCEINLINE inline
#endif

#if defined(__GNUC__)
	#define RAMEN_WARN_UNUSED_RESULT __attribute__ (( warn_unused_result))
#endif

#endif
