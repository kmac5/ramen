// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_CUDA_CUDART_HPP
#define RAMEN_CUDA_CUDART_HPP

#include<ramen/cuda/runtime_exceptions.hpp>

namespace ramen
{
namespace cuda
{

// error handling
void check_cuda_error( cudaError_t err);

// forwarding funs

void *cuda_malloc( std::size_t size);
void *cuda_malloc_host( std::size_t size);

void *cuda_malloc_pitch( int width, int height, std::size_t& pitch);

void cuda_free( void *ptr);
void cuda_free_host( void *ptr);

void cuda_memcpy( void *dst, void *src, std::size_t count, enum cudaMemcpyKind kind);
void cuda_memcpy_2d( void *dst, std::size_t dpitch, const void *src, std::size_t spitch,
					 std::size_t width, std::size_t height, enum cudaMemcpyKind kind);

} // namespace
} // namespace

#endif
