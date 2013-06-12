// Copyright (c) 2013 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_CORE_DICTIONARY_HPP
#define RAMEN_CORE_DICTIONARY_HPP

#include<ramen/core/dictionary_fwd.hpp>

#include<boost/move/move.hpp>

#include<ramen/core/name.hpp>
#include<ramen/core/variant.hpp>

namespace ramen
{
namespace core
{

/*!
\ingroup core
\brief A collection of variant_t indexed by name_t keys.
*/
class RAMEN_API dictionary_t
{
    BOOST_COPYABLE_AND_MOVABLE( dictionary_t)

public:

    typedef name_t      key_type;
    typedef variant_t   value_type;

    typedef std::pair<key_type, value_type>   pair_type;
    typedef std::size_t                       size_type;

    typedef pair_type*         iterator;
    typedef const pair_type*   const_iterator;

    dictionary_t();
    ~dictionary_t();

    // Copy constructor
    dictionary_t( const dictionary_t& other);

    // Copy assignment
    dictionary_t& operator=( BOOST_COPY_ASSIGN_REF( dictionary_t) other)
    {
        dictionary_t tmp( other);
        swap( tmp);
        return *this;
    }

    // Move constructor
    dictionary_t( BOOST_RV_REF( dictionary_t) other) : pimpl_( 0)
    {
        swap( other);
    }

    // Move assignment
    dictionary_t& operator=( BOOST_RV_REF( dictionary_t) other)
    {
        swap( other);
        return *this;
    }

    void swap( dictionary_t& other);

    bool empty() const;
    size_type size() const;

    void clear();

    const value_type& operator[]( const key_type& key) const;
    value_type& operator[]( const key_type& key);

    const value_type *get( const key_type& key) const;
    value_type *get( const key_type& key);

    struct dict_inserter
    {
        dict_inserter( dictionary_t& d) : d_( d) {}

        const dict_inserter& operator()( const dictionary_t::key_type& key, const value_type& val) const
        {
            d_[key] = val;
            return *this;
        }

        template<class T>
        const dict_inserter& operator()( const dictionary_t::key_type& key, const T& val) const
        {
            return (*this)( key, dictionary_t::value_type( val));
        }

        template<class T>
        const dict_inserter& operator()( const char *key, const T& val) const
        {
            return (*this)( dictionary_t::key_type( key), dictionary_t::value_type( val));
        }

    private:

        dictionary_t& d_;
    };

    dict_inserter insert()
    {
        return dict_inserter( *this);
    }

    // iterators
    const_iterator begin() const;
    const_iterator end() const;

    iterator begin();
    iterator end();

    bool operator==( const dictionary_t& other) const;
    bool operator!=( const dictionary_t& other) const;

private:

    struct impl;
    impl *pimpl_;
};

inline void swap( dictionary_t& x, dictionary_t& y)
{
    x.swap( y);
}

template<class T>
const T& get( const dictionary_t& dic, const dictionary_t::key_type& key)
{
    return get<T>( dic[key]);
}

template<class T>
T& get( dictionary_t& dic, const dictionary_t::key_type& key)
{
    return get<T>( dic[key]);
}

template<class T>
const T *get( const dictionary_t *dic, const dictionary_t::key_type& key)
{
    if( const dictionary_t::value_type *v = dic->get( key))
        return get<T>( v);

    return 0;
}

template<class T>
T *get( dictionary_t *dic, const dictionary_t::key_type& key)
{
    if( dictionary_t::value_type *v = dic->get( key))
        return get<T>( v);

    return 0;
}

template<class T>
bool get( const dictionary_t& dic, const dictionary_t::key_type& key, T& val)
{
    if( const dictionary_t::value_type *v = dic.get( key))
    {
        if( const T *x = get<T>( v))
        {
            val = *x;
            return true;
        }
    }

    return false;
}

template<class T>
T get_optional( const dictionary_t& dic, const dictionary_t::key_type& key, const T& default_value)
{    
    if( const dictionary_t::value_type *v = dic.get( key))
    {
        if( const T *x = get<T>( v))
            return *x;
    }

    return default_value;
}

} // core
} // ramen

#endif
