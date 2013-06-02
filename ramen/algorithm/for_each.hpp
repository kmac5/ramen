// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_ALGORITHM_FOREACH_HPP
#define RAMEN_ALGORITHM_FOREACH_HPP

namespace ramen
{

template <class InputIterator1, class InputIterator2, class BinaryFunction>
BinaryFunction for_each( InputIterator1 first, InputIterator1 last, 
						 InputIterator2 first2, BinaryFunction f)
{
	for( ; first != last; ++first, ++first2)
		f( *first, *first2);
	
	return f;
}

} // ramen

#endif
