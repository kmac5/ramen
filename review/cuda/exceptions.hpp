// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_CUDA_EXCEPTIONS_HPP
#define RAMEN_CUDA_EXCEPTIONS_HPP

#include<stdexcept>

namespace ramen
{
namespace cuda
{
	
class error : public std::runtime_error
{
public:
	
	explicit error( const std::string& msg);
};

struct out_of_memory : std::bad_alloc {};

} // namespace
} // namespace

#endif
