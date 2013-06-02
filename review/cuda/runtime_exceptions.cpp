// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/cuda/runtime_exceptions.hpp>

#include<ramen/assert.hpp>

namespace ramen
{
namespace cuda
{

runtime_error::runtime_error( cudaError_t err) : error( cuda_error_to_str( err)), error_( err) {}

const char *runtime_error::cuda_error_to_str( cudaError_t e)
{
	switch( e)
	{
		case cudaSuccess:
		{
			RAMEN_ASSERT( 0);
			return "success";
		}

		case cudaErrorMemoryAllocation:
		{
			RAMEN_ASSERT( 0);
			return "out of memory";
		}

		default:
			return "unknown";
	};
}

} // namespace
} // namespace
