// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_CUDA_MANAGER_HPP
#define RAMEN_CUDA_MANAGER_HPP

#include<vector>

#include<boost/noncopyable.hpp>
#include<boost/thread/mutex.hpp>

#include<loki/Singleton.h>

#include<ramen/cuda/cuda.hpp>
#include<ramen/cuda/cudart.hpp>

#include<ramen/cuda/device.hpp>

namespace ramen
{
namespace cuda
{

bool initialized();

class context_lock_t : boost::noncopyable
{
public:

	context_lock_t();
	~context_lock_t();
};

class manager_impl : boost::noncopyable
{
public:
	
	const std::vector<device_t>& devices() const { return devices_;}
	const device_t& current_device() const		 { return devices_[current_device_];}

private:

    friend struct Loki::CreateUsingNew<manager_impl>;
	friend class context_lock_t;

    manager_impl();
	~manager_impl();
	
	void get_devices();
	void choose_device();

	friend bool initialized();

	bool initialized_;

	int driver_version_;
	int runtime_version_;

	std::vector<device_t> devices_;
	int current_device_;

	CUcontext context_;
	boost::mutex mutex_;
};

typedef Loki::SingletonHolder<manager_impl> manager_t;

} // namespace
} // namespace

#endif
