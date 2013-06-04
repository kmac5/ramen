// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/cuda/cuda.hpp>

#include<boost/function.hpp>

#include<ramen/assert.hpp>

#include<ramen/cuda/delay_load_libcuda.hpp>

namespace ramen
{
namespace cuda
{
namespace
{

boost::function<CUresult ( unsigned int)> cuInit_;
boost::function<CUresult ( int*)> cuDriverGetVersion_;
boost::function<CUresult ( CUdevice *, int)> cuDeviceGet_;

boost::function<CUresult ( CUcontext*, unsigned int, CUdevice)> cuCtxCreate_;
boost::function<CUresult ( CUcontext)> cuCtxDestroy_;
boost::function<CUresult ( CUcontext)> cuCtxPushCurrent_;
boost::function<CUresult ( CUcontext*)> cuCtxPopCurrent_;
boost::function<CUresult ( CUcontext*)> cuCtxGetCurrent_;

void reset_all_function_pointers()
{
	cuInit_ = 0;
	cuDriverGetVersion_ = 0;
	cuDeviceGet_ = 0;
	cuCtxCreate_ = 0;
	cuCtxDestroy_ = 0;
	cuCtxPushCurrent_ = 0;
	cuCtxPopCurrent_ = 0;
	cuCtxGetCurrent_ = 0;
}

bool delay_load_libcuda_internal()
{
	if( !do_load_libcuda())
		return false;

	// init all function pointers here
	if( !( cuInit_ = reinterpret_cast<CUresult (*)( unsigned int)>( get_proc_ex( "cuInit"))))
		return false;

	if( !( cuDriverGetVersion_ = reinterpret_cast<CUresult (*)( int *)>( get_proc_ex( "cuDriverGetVersion"))))
		return false;

	if( !( cuDeviceGet_ = reinterpret_cast<CUresult (*)( CUdevice *, int)>( get_proc_ex( "cuDeviceGet"))))
		return false;

	if( !( cuCtxCreate_ = reinterpret_cast<CUresult (*)( CUcontext*, unsigned int, CUdevice)>( get_proc_ex_v2( "cuCtxCreate"))))
		return false;

	if( !( cuCtxDestroy_ = reinterpret_cast<CUresult (*)( CUcontext)>( get_proc_ex_v2( "cuCtxDestroy"))))
		return false;

	if( !( cuCtxPushCurrent_ = reinterpret_cast<CUresult (*)( CUcontext)>( get_proc_ex_v2( "cuCtxPushCurrent"))))
		return false;

	if( !( cuCtxPopCurrent_ = reinterpret_cast<CUresult (*)( CUcontext*)>( get_proc_ex_v2( "cuCtxPopCurrent"))))
		return false;

	if( !( cuCtxGetCurrent_ = reinterpret_cast<CUresult (*)( CUcontext*)>( get_proc_ex( "cuCtxGetCurrent"))))
		return false;

	return true;
}

} // unnamed

// initialize
bool delay_load_libcuda()
{
	bool result = delay_load_libcuda_internal();

	if( !result)
		reset_all_function_pointers();

	return result;
}

void unload_libcuda()
{
	reset_all_function_pointers();
	do_unload_libcuda();
}

void check_cu_error( CUresult err)
{
	switch( err)
	{
		case CUDA_SUCCESS:
			return;

		case CUDA_ERROR_OUT_OF_MEMORY:
			throw out_of_memory();

		default:
			throw driver_error( err);
	};
}

void cu_init( unsigned int flags)
{
	RAMEN_ASSERT( cuInit_);
	check_cu_error( cuInit_( flags));
}

void cu_device_get( CUdevice *dev, int ordinal)
{
	RAMEN_ASSERT( cuDeviceGet_);
	check_cu_error( cuDeviceGet_( dev, ordinal));
}

void cu_ctx_create( CUcontext *pctx, unsigned int flags, CUdevice dev)
{
	RAMEN_ASSERT( cuCtxCreate_);
	check_cu_error( cuCtxCreate_( pctx, flags, dev));
}

void cu_ctx_destroy( CUcontext ctx)
{
	RAMEN_ASSERT( cuCtxDestroy_);

	// in this case, we don't check for errors, as
	// this is used in a destructor, and there's not much
	// we can do about it anyway.
	cuCtxDestroy_( ctx);
}

void cu_ctx_push_current( CUcontext ctx)
{
	RAMEN_ASSERT( cuCtxPushCurrent_);
	check_cu_error( cuCtxPushCurrent_( ctx));
}

void cu_ctx_pop_current( CUcontext *pctx)
{
	RAMEN_ASSERT( cuCtxPopCurrent_);
	check_cu_error( cuCtxPopCurrent_( pctx));
}

void cu_ctx_get_current( CUcontext *pctx)
{
	RAMEN_ASSERT( cuCtxGetCurrent_);
	check_cu_error( cuCtxGetCurrent_( pctx));
}

} // namespace
} // namespace
