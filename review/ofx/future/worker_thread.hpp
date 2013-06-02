// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_OFX_WORKER_THREAD_HPP
#define RAMEN_OFX_WORKER_THREAD_HPP

#include<boost/thread.hpp>

namespace ramen
{
namespace ofx
{

class worker_thread_t
{
public:

	worker_thread_t( int index);
	~worker_thread_t();
	
private:
	
	void thread_loop();

	boost::thread thread_;
    boost::mutex mutex_;
    boost::condition_variable cond_;
	
    bool finish;
    bool ready;
    bool do_work;
    bool cancel;
};
	
} // namespace
} // namespace

#endif
