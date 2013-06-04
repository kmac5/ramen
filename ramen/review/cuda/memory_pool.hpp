// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_CUDA_MEMORY_POOL_HPP
#define RAMEN_CUDA_MEMORY_POOL_HPP

#include<ramen/config.hpp>

#include<memory>

#include<boost/noncopyable.hpp>

namespace ramen
{
namespace cuda
{

class memory_pool_t : boost::noncopyable
{
public:

    memory_pool_t();
    ~memory_pool_t();

    void init( std::size_t size_in_bytes);

    std::size_t pool_size() const;

    unsigned char *allocate( std::size_t& size) RAMEN_WARN_UNUSED_RESULT;
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
