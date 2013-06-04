// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_CUDA_RUNTIME_EXCEPTIONS_HPP
#define RAMEN_CUDA_RUNTIME_EXCEPTIONS_HPP

#include<ramen/cuda/exceptions.hpp>

#include<cuda_runtime_api.h>

namespace ramen
{
namespace cuda
{

class runtime_error : public error
{
public:

	explicit runtime_error( cudaError_t err);

	cudaError_t cuda_error() const { return error_;}

protected:

	static const char *cuda_error_to_str( cudaError_t e);

	cudaError_t error_;
};

} // namespace
} // namespace

#endif
