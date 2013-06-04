// From Adobe source libraries. Original license follows.
/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

#ifndef RAMEN_REGULAR_CONCEPT_HPP
#define RAMEN_REGULAR_CONCEPT_HPP

#include<boost/concept_check.hpp>
#include<boost/swap.hpp>

namespace ramen
{

template <class T>
struct RegularConcept :	boost::CopyConstructible<T>,
						boost::Assignable<T>,
						boost::EqualityComparable<T>
{
	BOOST_CONCEPT_USAGE( RegularConcept)
	{
		boost::swap( t, t);
	}

	T t;
};

} // ramen

#endif
