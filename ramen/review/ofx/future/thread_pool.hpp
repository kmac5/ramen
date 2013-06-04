// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_OFX_THREAD_POOL_HPP
#define RAMEN_OFX_THREAD_POOL_HPP

#include<boost/noncopyable.hpp>

#include<loki/Singleton.h>

#include<ramen/ofx/worker_thread.hpp>

namespace ramen
{
namespace ofx
{

class thread_pool_impl : boost::noncopyable
{
public:

private:

    friend struct Loki::CreateUsingNew<thread_pool_impl>;
	
	thread_pool_impl();
	~thread_pool_impl();
};

typedef Loki::SingletonHolder<thread_pool_impl> host_t;

} // namespace
} // namespace

#endif
