// Copyright (c) 2013 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_ALGORITHM_FOR_EACH_POSITION_HPP
#define RAMEN_ALGORITHM_FOR_EACH_POSITION_HPP

#include<ramen/config.hpp>

#include<boost/bind.hpp>
#include<boost/range/begin.hpp>
#include<boost/range/end.hpp>

namespace ramen
{

template<class InputIterator, class UnaryFunction>
inline void for_each_position( InputIterator first, InputIterator last, UnaryFunction f)
{
    while( first != last)
    {
        f( first);
        ++first;
    }
}

template<class InputRange, class UnaryFunction>
inline void for_each_position( InputRange& range, UnaryFunction f)
{
    for_each_position( boost::begin( range), boost::end( range), f);
}

template<class InputRange, class UnaryFunction>
inline void for_each_position( const InputRange& range, UnaryFunction f)
{
    for_each_position( boost::begin( range), boost::end( range), f);
}

} // ramen

#endif
