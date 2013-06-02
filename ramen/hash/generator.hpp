// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_HASH_GENERATOR_HPP
#define	RAMEN_HASH_GENERATOR_HPP

#include<sstream>

#include<boost/optional.hpp>

#include<adobe/md5.hpp>

#include<ramen/assert.hpp>

namespace ramen
{
namespace hash
{

class generator_t
{
public:

    typedef adobe::md5_t::digest_t digest_type;

    generator_t();

    bool empty() const		{ return empty_;}
    void set_empty( bool b) { empty_ = b;}

    void reset();

    std::stringstream& sstream() { return ss_;}

    std::string str() const;

    bool finalized() const { return digest_;}

    const digest_type& digest() const;
    std::string digest_as_string() const;

    static digest_type digest_from_string( const std::string& s);

private:

    std::stringstream ss_;
    mutable boost::optional<digest_type> digest_;
    bool empty_;
};

template<class T>
generator_t& operator<<( generator_t& hash_gen, const T& x)
{
    RAMEN_ASSERT( !hash_gen.finalized());
    hash_gen.sstream() << x;

    hash_gen.set_empty( false);
    return hash_gen;
}

} // util
} // ramen

#endif

