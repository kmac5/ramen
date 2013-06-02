// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/cuda/cudart.hpp>

namespace ramen
{
namespace cuda
{

void check_cuda_error( cudaError_t err)
{
	switch( err)
	{
		case cudaSuccess:
			return;

		case cudaErrorMemoryAllocation:
			throw out_of_memory();

		default:
			throw runtime_error( err);
	}
}

void *cuda_malloc( std::size_t size)
{
	void *devPtr;
	check_cuda_error( cudaMalloc( &devPtr, size));
	return devPtr;
}

void *cuda_malloc_host( std::size_t size)
{
	void *dev_ptr;
	check_cuda_error( cudaMallocHost( &dev_ptr, size));
	return dev_ptr;
}

void *cuda_malloc_pitch( int width, int height, std::size_t& pitch)
{
	void *dev;
	size_t p;
	check_cuda_error( cudaMallocPitch( &dev, &p, width, height));
	pitch = p;
	return dev;
}

void cuda_free( void *ptr)		{ check_cuda_error( cudaFree( ptr));}
void cuda_free_host( void *ptr) { check_cuda_error( cudaFreeHost( ptr));}

void cuda_memcpy( void *dst, void *src, std::size_t count, enum cudaMemcpyKind kind)
{
	check_cuda_error( cudaMemcpy( dst, src, count, kind));
}

void cuda_memcpy_2d( void *dst, std::size_t dpitch, const void *src, std::size_t spitch,
					 std::size_t width, std::size_t height, enum cudaMemcpyKind kind)
{
	check_cuda_error( cudaMemcpy2D( dst, dpitch, src, spitch, width, height, kind));
}

} // namespace
} // namespace
