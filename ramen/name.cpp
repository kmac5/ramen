// Copyright (c) 2013 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/name.hpp>

#include<string>
#include<algorithm>

#include<boost/unordered_set.hpp>
#include<boost/thread/mutex.hpp>
#include<boost/thread/locks.hpp>

#include<ramen/assert.hpp>

namespace ramen
{
namespace
{

struct name_pool_t
{
    typedef boost::unordered::unordered_set<std::string> name_set_type;
    typedef name_set_type::const_iterator	const_iterator;
    typedef name_set_type::iterator			iterator;

    typedef boost::mutex                    mutex_type;
    typedef boost::lock_guard<mutex_type>   lock_type;

    name_pool_t()
    {
        g_empty_string = add( std::string( ""));
    }

    const char *add( const std::string& str)
    {
        lock_type lock( mutex_);

        const_iterator it( names_.find( str));

        if( it != names_.end())
            return it->c_str();

        std::pair<iterator, bool> result = names_.insert( str);
        return result.first->c_str();
    }

    mutex_type mutex_;
    name_set_type names_;
    const char *g_empty_string;
};

struct singleton_name_pool_holder_t
{
    static name_pool_t& get()
    {
        static name_pool_t pool;
        return pool;
    }
};

} // unnamed

name_t::name_t() { init( "");}

name_t::name_t( const char *str)
{
    RAMEN_ASSERT( str);

    init( str);
}

void name_t::init( const char *str)
{
    data_ = singleton_name_pool_holder_t::get().add( std::string( str));
}

name_t& name_t::operator=( const name_t& other)
{
    name_t tmp( other);
    swap( tmp);
    return *this;
}

bool name_t::empty() const
{
    return c_str() == singleton_name_pool_holder_t::get().g_empty_string;
}

} // ramen
