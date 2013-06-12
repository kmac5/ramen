// Copyright (c) 2013 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/core/variant.hpp>

namespace ramen
{
namespace core
{

template<class T>
struct variant_t::vtable_impl
{
    static type_t type()
    {
        return type_traits<T>::type();
    }

    static void destroy( const variant_t& v)
    {
        const T& data = *reinterpret_cast<const T*>( v.storage());
        data.~T();
    }

    static void clone( const variant_t& src, variant_t& dst)
    {
        new( dst.storage()) T( *reinterpret_cast<const T*>( src.storage()));
        dst.vptr_ = src.vptr_;
    }

    static bool equals( const variant_t& a, const variant_t& b)
    {
        const T& a_data = *reinterpret_cast<const T*>( a.storage());
        const T& b_data = *reinterpret_cast<const T*>( b.storage());
        return a_data == b_data;
    }
};

// Needs special handling, as string_t copy constructor can throw.
template<>
struct variant_t::vtable_impl<string_t>
{
    static type_t type()
    {
        return type_traits<string_t>::type();
    }

    static void destroy(const variant_t& v)
    {
        const string_t& data = *reinterpret_cast<const string_t*>( v.storage());
        data.~string_t();
    }

    static void clone(const variant_t& src, variant_t& dst)
    {
        try
        {
            // this can throw, leave dst untouched if it does.
            string_t tmp( *reinterpret_cast<const string_t*>( src.storage()));

            // From here, nothing can throw.
            new( dst.storage()) string_t();
            string_t& dst_str( *reinterpret_cast<string_t*>( dst.storage()));
            dst_str = boost::move( tmp);
            dst.vptr_ = src.vptr_;
        }
        catch( ...)
        {
            throw;
        }
    }

    static bool equals( const variant_t& a, const variant_t& b)
    {
        const string_t& a_data = *reinterpret_cast<const string_t*>( a.storage());
        const string_t& b_data = *reinterpret_cast<const string_t*>( b.storage());
        return a_data == b_data;
    }
};

variant_t::variant_t()                              { init<bool>( false);}
variant_t::variant_t( bool x)                       { init<bool>( x);}
variant_t::variant_t( boost::int32_t x)             { init<boost::int32_t>( x);}
variant_t::variant_t( boost::uint32_t x)            { init<boost::uint32_t>( x);}
variant_t::variant_t( float x)                      { init<float>( x);}
variant_t::variant_t( const math::point2i_t& x)     { init<math::point2i_t>( x);}
variant_t::variant_t( const math::point2f_t& x)     { init<math::point2f_t>( x);}
variant_t::variant_t( const math::point3f_t& x)     { init<math::point3f_t>( x);}
variant_t::variant_t( const math::hpoint3f_t& x)    { init<math::hpoint3f_t>( x);}
variant_t::variant_t( const string_t& x)            { init<string_t>( x);}
variant_t::variant_t( const char *x)                { init<string_t>( x);}
variant_t::variant_t( char *x)                      { init<string_t>( x);}
variant_t::variant_t( const math::box2i_t& x)       { init<math::box2i_t>( x);}

variant_t::variant_t( const variant_t& other)
{
    other.vptr_->clone( other, *this);
}

variant_t::~variant_t()
{
    vptr_->destroy( *this);
}

variant_t& variant_t::operator=( const variant_t& other)
{
    vptr_->destroy( *this);
    other.vptr_->clone( other, *this);
    return *this;
}

type_t variant_t::type() const
{
    return vptr_->type();
}

template<class T>
void variant_t::init( const T& x)
{
    new( storage()) T( x);

    static vtable vtbl =
    {
        &vtable_impl<T>::type,
        &vtable_impl<T>::destroy,
        &vtable_impl<T>::clone,
        &vtable_impl<T>::equals
    };

    vptr_ = &vtbl;
}

bool variant_t::operator==( const variant_t& other) const
{
    return type() == other.type() && vptr_->equals( *this, other);
}

bool variant_t::operator!=( const variant_t& other) const
{
    return !( *this == other);
}

const unsigned char *variant_t::storage() const
{
    return &storage_[0];
}

unsigned char *variant_t::storage()
{
    return &storage_[0];
}

std::ostream& operator<<( std::ostream& os, const variant_t& x)
{
    switch( x.type())
    {
        case bool_k:
            os << get<bool>( x);
        break;

        case int32_k:
            os << get<boost::int32_t>( x);
        break;

        case uint32_k:
            os << get<boost::uint32_t>( x);
        break;

        case float_k:
            os << get<float>( x);
        break;

        case string_k:
            os << get<string_t>( x);
        break;

        default:
            throw core::not_implemented();
    }

    return os;
}

} // core
} // ramen
