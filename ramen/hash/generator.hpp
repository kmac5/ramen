// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_HASH_GENERATOR_HPP
#define	RAMEN_HASH_GENERATOR_HPP

#include<ramen/config.hpp>

#include<sstream>

#include<boost/cstdint.hpp>
#include<boost/array.hpp>
#include<boost/optional.hpp>

#include<ramen/assert.hpp>

namespace ramen
{
namespace hash
{

class RAMEN_API generator_t
{
public:

    typedef boost::array<boost::uint64_t, 2> digest_type;

    generator_t();

    bool empty() const		{ return empty_;}
    void set_empty( bool b) { empty_ = b;}

    void reset();

    std::stringstream& sstream() { return ss_;}

    std::string str() const;

    bool finalized() const
    {
        return digest_;
    }

    const digest_type& digest() const;
    std::string digest_as_string() const;

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

} // hash
} // ramen

#endif
