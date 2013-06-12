// Copyright (c) 2013 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_CORE_VARIANT_HPP
#define RAMEN_CORE_VARIANT_HPP

#include<ramen/config.hpp>

#include<iostream>

#include<boost/mpl/vector.hpp>
#include<boost/mpl/max_element.hpp>
#include<boost/mpl/transform_view.hpp>
#include<boost/mpl/sizeof.hpp>
#include<boost/mpl/deref.hpp>
#include<boost/mpl/copy.hpp>
#include<boost/mpl/back_inserter.hpp>
#include<boost/mpl/assert.hpp>
#include<boost/mpl/size.hpp>
#include<boost/mpl/equal.hpp>

#include<boost/static_assert.hpp>
#include<boost/type_traits/is_pointer.hpp>

#include<ramen/core/types.hpp>
#include<ramen/core/exceptions.hpp>

namespace ramen
{
namespace core
{

/*!
\ingroup core
\brief A discriminated union class.
*/
class RAMEN_API variant_t
{
public:

    variant_t();

    explicit variant_t( bool x);

    explicit variant_t( boost::int32_t x);
    explicit variant_t( boost::uint32_t x);

    explicit variant_t( float x);

    explicit variant_t( const math::point2i_t& x);
    explicit variant_t( const math::point2f_t& x);
    explicit variant_t( const math::point3f_t& x);
    explicit variant_t( const math::hpoint3f_t& x);

    explicit variant_t( const string_t& x);

    explicit variant_t( const char *x);
    explicit variant_t( char *x);

    explicit variant_t( const math::box2i_t& x);

    // when variant is constructed with a pointer, generate a compile error
    // to avoid the implicit conversion to bool.
    template<class T>
    explicit variant_t( T*)
    {
        BOOST_STATIC_ASSERT( boost::mpl::not_<boost::is_pointer<T*> >::value);
    }

    variant_t( const variant_t& other);

    ~variant_t();

    variant_t& operator=( const variant_t& other);

    type_t type() const;

    bool operator==( const variant_t& other) const;
    bool operator!=( const variant_t& other) const;

private:

    template<class X>
    friend const X& get( const variant_t& v);

    template<class X>
    friend X& get( variant_t& v);

    template<class X>
    friend const X *get( const variant_t *v);

    template<class X>
    friend X *get( variant_t *v);

    template<class T>
    void init( const T& x);

    const unsigned char *storage() const;
    unsigned char *storage();

    template<class T>
    struct make_struct
    {
        T unused;
    };

    struct string_struct
    {
        const char *ptr;
        std::size_t size;
    };

    template<class T>
    struct box2_struct
    {
        make_struct<T[2]> a;
        make_struct<T[2]> b;
    };

    template<class T>
    struct box3_struct
    {
        make_struct<T[3]> a;
        make_struct<T[3]> b;
    };

    union max_align
    {
        bool b;
        boost::int32_t i32;
        boost::int64_t i64;
        float f;

        make_struct<int[2]>   i2;

        make_struct<float[2]> f2;
        make_struct<float[3]> f3;
        make_struct<float[4]> f4;

        string_struct ss;

        box2_struct<int> bis2;
        box2_struct<float> bfs2;

        box3_struct<int> bis3;
        box3_struct<float> bfs3;
    };

    typedef boost::mpl::vector< bool,
                                boost::int32_t,
                                boost::int64_t,
                                float,
                                math::point2i_t,
                                math::point2f_t,
                                math::point3f_t,
                                math::hpoint3f_t,
                                string_t,
                                math::box2i_t> type_list_t;

    // find the biggest element
    typedef boost::mpl::max_element<boost::mpl::transform_view<type_list_t,
                                                               boost::mpl::sizeof_<boost::mpl::_1> > >::type biggest_type_it;

    union
    {
        unsigned char storage_[sizeof( boost::mpl::deref<biggest_type_it::base>::type)];
        max_align unused;
    };

    struct vtable
    {
        type_t (*type)();
        void (*destroy)( const variant_t&);
        void (*clone)( const variant_t&, variant_t&);
        bool (*equals)( const variant_t&, const variant_t&);
    };

    template<class T>
    struct vtable_impl;

    vtable *vptr_;
};

template<class T>
const T& get( const variant_t& v)
{
    if( v.type() != type_traits<T>::type())
        throw bad_type_cast( v.type(), type_traits<T>::type());

    return *reinterpret_cast<const T*>( v.storage());
}

template<class T>
T& get( variant_t& v)
{
    if( v.type() != type_traits<T>::type())
        throw bad_type_cast( v.type(), type_traits<T>::type());

    return *reinterpret_cast<T*>( v.storage());
}

template<class T>
const T *get( const variant_t *v)
{
    if( v->type() != type_traits<T>::type())
        return 0;

    return reinterpret_cast<const T*>( v->storage());
}

template<class T>
T *get( variant_t *v)
{
    if( v->type() != type_traits<T>::type())
        return 0;

    return reinterpret_cast<T*>( v->storage());
}

// Equality
template<class T>
bool operator==( const variant_t& a, const T& b)
{
    if( type_traits<T>::type() != a.type())
        return false;

    return get<T>( a) == b;
}

template<class T>
bool operator==( const variant_t& a, const char *b)
{
    if( a.type() != string_k)
        return false;

    return get<T>( a) == b;
}

template<class T>
bool operator==( const T& a, const variant_t& b) { return b == a;}

template<class T>
bool operator!=( const variant_t& a, const T& b) { return !( a == b);}

template<class T>
bool operator!=( const T& a, const variant_t& b) { return !( b == a);}

RAMEN_API std::ostream& operator<<( std::ostream& os, const variant_t& x);

} // core
} // ramen

#endif
