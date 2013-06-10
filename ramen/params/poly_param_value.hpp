// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_PARAM_VALUE_HPP
#define RAMEN_PARAM_VALUE_HPP

#include<boost/static_assert.hpp>
#include<boost/type_traits/is_same.hpp>

#include<adobe/poly_regular.hpp>

#include<ramen/params/Concepts/ParamValueConcept.hpp>

namespace ramen
{

struct poly_param_value_interface : adobe::poly_regular_interface
{
};

template <typename T>
struct poly_param_value_instance : adobe::optimized_storage_type<T, poly_param_value_interface>::type
{
private:

    BOOST_CONCEPT_ASSERT(( ParamValueConcept<T>));

public:

    typedef typename adobe::optimized_storage_type<T, poly_param_value_interface>::type base_t;

    poly_param_value_instance(const T& x) : base_t( x) {}
    poly_param_value_instance( adobe::move_from<poly_param_value_instance> x) : base_t( adobe::move_from<base_t>( x.source)) {}

    bool equals( const adobe::poly_regular_interface& x) const
    {
        return this->type_info() == x.type_info() && this->get() == *static_cast<const T*>( x.cast());
    }
};

struct param_value : adobe::poly_base<poly_param_value_interface, poly_param_value_instance>
{
    typedef adobe::poly_base<poly_param_value_interface, poly_param_value_instance> base_t;

    param_value() : base_t( adobe::empty_t()) {}

    template <typename T>
    explicit param_value( const T& s) : base_t( s) {}

    param_value( adobe::move_from<param_value> x) : base_t( adobe::move_from<base_t>(x.source)) {}

    bool is_empty() const { return type_info() == typeid( adobe::empty_t);}
};

typedef adobe::poly<param_value> poly_param_value_t;

// Indexable values ( vectors, colors, ...)
struct poly_param_indexable_value_interface : poly_param_value_interface
{
    virtual float get_component( int index) const = 0;
    virtual void set_component( int index, float x) = 0;
};

template <typename T>
struct poly_param_indexable_value_instance : adobe::optimized_storage_type<T, poly_param_indexable_value_interface>::type
{
private:

    BOOST_CONCEPT_ASSERT(( IndexableParamValueConcept<T>));

public:

    typedef typename adobe::optimized_storage_type<T, poly_param_indexable_value_interface>::type base_t;

    poly_param_indexable_value_instance(const T& x) : base_t( x) {}
    poly_param_indexable_value_instance( adobe::move_from<poly_param_indexable_value_instance> x) : base_t( adobe::move_from<base_t>( x.source)) {}

    bool equals( const adobe::poly_regular_interface& x) const
    {
        return this->type_info() == x.type_info() && this->get() == *static_cast<const T*>( x.cast());
    }

    float get_component( int index) const
    {
        assert( this->type_info() != typeid( adobe::empty_t()));
        assert( index >= 0);
        assert( index * sizeof( float) < sizeof( T));

        const float *ptr = reinterpret_cast<const float *>( &( this->get()));
        return ptr[ index];
    }

    void set_component( int index, float x)
    {
        assert( this->type_info() != typeid( adobe::empty_t()));
        assert( index >= 0);
        assert( index * sizeof( float) < sizeof( T));

        float *ptr = reinterpret_cast<float *>( &( this->get()));
        ptr[ index] = x;
    }
};

struct param_indexable_value : adobe::poly_base<poly_param_indexable_value_interface, poly_param_indexable_value_instance>
{
    typedef adobe::poly_base<poly_param_indexable_value_interface, poly_param_indexable_value_instance> base_t;

    param_indexable_value() : base_t( adobe::empty_t()) {}

    template <typename T>
    explicit param_indexable_value( const T& s) : base_t( s) {}

    param_indexable_value( adobe::move_from<param_indexable_value> x) : base_t( adobe::move_from<base_t>(x.source)) {}

    bool is_empty() const { return type_info() == typeid( adobe::empty_t);}

    float get_component( int index) const   { return interface_ref().get_component( index);}
    void set_component( int index, float x) { interface_ref().set_component( index, x);}
};

typedef adobe::poly<param_indexable_value> poly_param_indexable_value_t;

} // namespace

#endif
