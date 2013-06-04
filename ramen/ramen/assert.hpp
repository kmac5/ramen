// Copied from boost/assert. We couldn't use the original, as it would affect boost's headers.
// So we copy and rename it. Original license follows...

//  boost/assert.hpp - BOOST_ASSERT(expr)
//
//  Copyright (c) 2001, 2002 Peter Dimov and Multi Media Ltd.
//  Copyright (c) 2007 Peter Dimov
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
//  Note: There are no include guards. This is intentional.
//
//  See http://www.boost.org/libs/utility/assert.html for documentation.
//

#undef RAMEN_ASSERT

#if defined( RAMEN_DISABLE_ASSERTS)

# define RAMEN_ASSERT( expr) ( (void)0)

#elif defined( RAMEN_ENABLE_ASSERT_HANDLER)

#include <boost/current_function.hpp>

#include<ramen/config.hpp>

namespace ramen
{

void RAMEN_API assertion_failed(char const * expr, char const * function, char const * file, long line); // user defined

} // namespace ramen

#define RAMEN_ASSERT(expr) ((expr)? ((void)0): ::ramen::assertion_failed(#expr, BOOST_CURRENT_FUNCTION, __FILE__, __LINE__))

#else
# include <assert.h> // .h to support old libraries w/o <cassert> - effect is the same
# define RAMEN_ASSERT( expr) assert( expr)
#endif

#undef RAMEN_VERIFY

#if defined( RAMEN_DISABLE_ASSERTS) || ( !defined( RAMEN_ENABLE_ASSERT_HANDLER) && defined( NDEBUG) )

# define RAMEN_VERIFY( expr) ((void)(expr))

#else

# define RAMEN_VERIFY( expr) RAMEN_ASSERT( expr)

#endif
