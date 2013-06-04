// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_CONTAINER_PTR_VECTOR_UTIL_HPP
#define RAMEN_CONTAINER_PTR_VECTOR_UTIL_HPP

#include<memory>
#include<algorithm>

#include<boost/ptr_container/ptr_vector.hpp>

#include<ramen/assert.hpp>

namespace ramen
{
namespace container
{

template<class T, class C, class A>
std::auto_ptr<T> release_ptr( T *p, boost::ptr_vector<T,C,A>& vec)
{
    std::auto_ptr<T> result;

	for( typename boost::ptr_vector<T,C,A>::iterator it( vec.begin()), e( vec.end()); it != e; ++it)
	{
		if( &(*it) == p)
		{
			typename boost::ptr_vector<T,C,A>::auto_type ptr = vec.release( it);
            result.reset( ptr.release());
            break;
		}
	}

	return result;
}

template<class Iter, class T, class C, class A>
void swap_elements( Iter first, Iter second, boost::ptr_vector<T,C,A>& vec)
{
    std::swap( *first.base(), *second.base());
}

} // container
} // ramen

#endif
