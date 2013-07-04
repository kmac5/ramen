// From Adobe source libraries. Original license follows.
/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

#ifndef RAMEN_POLY_REGULAR_HPP
#define RAMEN_POLY_REGULAR_HPP

#include<ramen/Concepts/RegularConcept.hpp>

#include<ramen/core/poly.hpp>
#include<ramen/core/empty.hpp>

namespace ramen
{
namespace core
{

struct poly_regular_interface : poly_copyable_interface
{
    virtual bool equals( const poly_regular_interface& new_value) const = 0;
};

template <typename T>
class poly_regular_instance : public optimized_storage_type<T, poly_regular_interface>::type
{
    BOOST_CONCEPT_ASSERT(( RegularConcept<T>));

public:

    typedef typename optimized_storage_type<T, poly_regular_interface>::type base_type;

    explicit poly_regular_instance( const T& x) : base_type( x) {}

    explicit poly_regular_instance( BOOST_RV_REF( T) x) : base_type( x) {}

    RAMEN_POLY_INLINE_COPY_AND_ASSIGN( poly_regular_instance, base_type)

    bool equals( const poly_regular_interface& x) const
    {
        return this->type_info() == x.type_info() && this->get() == *static_cast<const T*>(x.cast());
    }
};

class regular : public poly_base<poly_regular_interface, poly_regular_instance>
{
public:

    typedef poly_base<poly_regular_interface, poly_regular_instance> base_type;

    regular() : base_type( empty_t()) {}

    template <typename T>
    explicit regular( const T& x) : base_type( x) {}

    template<typename T>
    explicit regular( BOOST_RV_REF( T) x) : base_type( x) {}

    RAMEN_POLY_INLINE_COPY_AND_ASSIGN( regular, base_type)

    bool is_empty() const { return type_info() == typeid( empty_t);}
};

typedef poly<regular> poly_regular_t;

} // core
} // ramen

#endif
