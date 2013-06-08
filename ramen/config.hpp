// Copyright (c) 2013 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_CONFIG_HPP
#define RAMEN_CONFIG_HPP

#include<ramen/config/os.hpp>
#include<ramen/config/compiler.hpp>
#include<ramen/config/visibility.hpp>

#ifdef ramen_EXPORTS
    #define RAMEN_API RAMEN_EXPORT
#else
    #define RAMEN_API RAMEN_IMPORT
#endif

// windows specific
#if defined( RAMEN_CONFIG_OS_WINDOWS)
     // disable min/max macros on windows.
    #define NOMINMAX
#endif

#endif
