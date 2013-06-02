// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/ofx/worker_thread.hpp>

#include<boost/bind.hpp>

namespace ramen
{
namespace ofx
{

worker_thread_t::worker_thread_t( int index)
{
	thread = boost::thread( &worker_thread_t::thread_loop, this);
}

worker_thread_t::~worker_thread_t()
{
}

void worker_thread_t::thread_loop()
{
}

} // namespace
} // namespace
