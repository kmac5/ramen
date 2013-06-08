// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_MEMORY_POOL_HPP
#define RAMEN_MEMORY_POOL_HPP

#include<ramen/config.hpp>

#include<memory>

#include<boost/noncopyable.hpp>

namespace ramen
{
namespace memory
{

class pool_t : boost::noncopyable
{
public:

    pool_t();
    ~pool_t();

    void init( std::size_t size_in_bytes);

    std::size_t pool_size() const;

    unsigned char *allocate( std::size_t& size);
    void deallocate( unsigned char *p, std::size_t size);

private:

    // implementation is private...
    struct implementation_t;
    std::auto_ptr<implementation_t> pimpl_;	
	std::size_t allocated_;
};

} // namespace
} // namespace

#endif
