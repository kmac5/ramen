// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/cuda/driver_exceptions.hpp>

#include<ramen/assert.hpp>

namespace ramen
{
namespace cuda
{

driver_error::driver_error( CUresult err) : error( curesult_to_str( err)), error_( err) {}

const char *driver_error::curesult_to_str( CUresult e)
{
	switch (e)
	{
		case CUDA_SUCCESS:
		{
			RAMEN_ASSERT( 0);
			return "success";
		}

		case CUDA_ERROR_OUT_OF_MEMORY:
		{
			RAMEN_ASSERT( 0);
			return "out of memory";
		}

		case CUDA_ERROR_INVALID_VALUE:
			return "invalid value";

		case CUDA_ERROR_NOT_INITIALIZED:
			return "not initialized";

		case CUDA_ERROR_DEINITIALIZED:
			return "deinitialized";

		case CUDA_ERROR_NO_DEVICE:
			return "no device";

		case CUDA_ERROR_INVALID_DEVICE:
			return "invalid device";

		case CUDA_ERROR_INVALID_IMAGE:
			return "invalid image";

		case CUDA_ERROR_INVALID_CONTEXT:
			return "invalid context";

		case CUDA_ERROR_CONTEXT_ALREADY_CURRENT:
			return "context already current";

		case CUDA_ERROR_MAP_FAILED:
			return "map failed";

		case CUDA_ERROR_UNMAP_FAILED:
			return "unmap failed";

		case CUDA_ERROR_ARRAY_IS_MAPPED:
			return "array is mapped";

		case CUDA_ERROR_ALREADY_MAPPED:
			return "already mapped";

		case CUDA_ERROR_NO_BINARY_FOR_GPU:
			return "no binary for gpu";

		case CUDA_ERROR_ALREADY_ACQUIRED:
			return "already acquired";

		case CUDA_ERROR_NOT_MAPPED:
			return "not mapped";

		case CUDA_ERROR_NOT_MAPPED_AS_ARRAY:
			return "not mapped as array";

		case CUDA_ERROR_NOT_MAPPED_AS_POINTER:
			return "not mapped as pointer";

		case CUDA_ERROR_ECC_UNCORRECTABLE:
			return "ECC uncorrectable";

		case CUDA_ERROR_UNSUPPORTED_LIMIT:
			return "unsupported limit";

		case CUDA_ERROR_INVALID_SOURCE:
			return "invalid source";

		case CUDA_ERROR_FILE_NOT_FOUND:
			return "file not found";

		case CUDA_ERROR_SHARED_OBJECT_SYMBOL_NOT_FOUND:
			return "shared object symbol not found";

		case CUDA_ERROR_SHARED_OBJECT_INIT_FAILED:
			return "shared object init failed";

		case CUDA_ERROR_INVALID_HANDLE:
			return "invalid handle";

		case CUDA_ERROR_NOT_FOUND:
			return "not found";

		case CUDA_ERROR_NOT_READY:
			return "not ready";

		case CUDA_ERROR_LAUNCH_FAILED:
			return "launch failed";

		case CUDA_ERROR_LAUNCH_OUT_OF_RESOURCES:
			return "launch out of resources";

		case CUDA_ERROR_LAUNCH_TIMEOUT:
			return "launch timeout";

		case CUDA_ERROR_LAUNCH_INCOMPATIBLE_TEXTURING:
			return "launch incompatible texturing";

		/*
		case CUDA_ERROR_POINTER_IS_64BIT:
			return "attempted to retrieve 64-bit pointer via 32-bit api function";

		case CUDA_ERROR_SIZE_IS_64BIT:
			return "attempted to retrieve 64-bit size via 32-bit api function";
		*/

		case CUDA_ERROR_UNKNOWN:
			return "unknown";

		default:
			return "invalid error code";
	}
}

} // namespace
} // namespace
