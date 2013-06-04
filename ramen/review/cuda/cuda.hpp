// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_CUDA_CUDA_HPP
#define RAMEN_CUDA_CUDA_HPP

#include<ramen/cuda/driver_exceptions.hpp>

namespace ramen
{
namespace cuda
{

// initialize
bool delay_load_libcuda();
void unload_libcuda();

// error handling
void check_cu_error( CUresult err);

// forwarding funs
void cu_init( unsigned int flags);
void cu_device_get( CUdevice *dev, int ordinal);

void cu_ctx_create( CUcontext *pctx, unsigned int flags, CUdevice dev);
void cu_ctx_destroy( CUcontext ctx);
void cu_ctx_push_current( CUcontext ctx);
void cu_ctx_pop_current( CUcontext *pctx);
void cu_ctx_get_current( CUcontext *pctx);

} // namespace
} // namespace

#endif
