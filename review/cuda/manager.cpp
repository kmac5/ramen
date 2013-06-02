// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/python/python.hpp>

#include<ramen/cuda/manager.hpp>

#include<ramen/assert.hpp>

#include<ramen/app/preferences.hpp>

namespace ramen
{
namespace cuda
{

bool initialized() { return manager_t::Instance().initialized_;}

context_lock_t::context_lock_t()
{
	RAMEN_ASSERT( initialized());

	manager_t::Instance().mutex_.lock();
	cu_ctx_push_current( manager_t::Instance().context_);
}

context_lock_t::~context_lock_t()
{
	CUcontext tmp;
	cu_ctx_pop_current( &tmp);
	manager_t::Instance().mutex_.unlock();
}

manager_impl::manager_impl()
{
	initialized_ = false;
	current_device_ = 0;

	get_devices();

	if( devices().empty())
		return;

	cudaDriverGetVersion( &driver_version_);
    cudaRuntimeGetVersion( &runtime_version_);

	if( delay_load_libcuda() == false)
		return;

	bool context_created = false;

	try
	{
		cu_init( 0);
		choose_device();
		CUdevice dev = current_device().get_cu_device();
		cu_ctx_create( &context_, 0, dev);
		context_created = true;
		initialized_ = true;
	}
	catch( ...)
	{
		if( context_created)
			cu_ctx_destroy( context_);

		unload_libcuda();
		initialized_ = false;
	}
}

manager_impl::~manager_impl()
{
	if( initialized_)
	{
		mutex_.lock();
		cu_ctx_destroy( context_);
		mutex_.unlock();
		unload_libcuda();
	}
}

void manager_impl::get_devices()
{
	int device_count;

	if( cudaGetDeviceCount( &device_count) != cudaSuccess)
		return;

    struct cudaDeviceProp properties;

	for( int i = 0; i < device_count; ++i)
	{
		cudaGetDeviceProperties( &properties, i);

		if( properties.major != 9999) /* 9999 means emulation only */
			devices_.push_back( device_t( i));
	}
}

void manager_impl::choose_device()
{
	// TODO: improve this...
	current_device_ = 0;
}

} // namespace
} // namespace
