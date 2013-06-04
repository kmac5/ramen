// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_CUDA_DRIVER_EXCEPTIONS_HPP
#define RAMEN_CUDA_DRIVER_EXCEPTIONS_HPP

#include<ramen/cuda/exceptions.hpp>

#include<cuda.h>

namespace ramen
{
namespace cuda
{

class driver_error : public error
{
public:

	explicit driver_error( CUresult err);

	CUresult cu_result() const { return error_;}

protected:

	static const char *curesult_to_str( CUresult e);

	CUresult error_;
};

} // namespace
} // namespace

#endif
