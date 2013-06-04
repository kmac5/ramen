// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/cuda/exceptions.hpp>

namespace ramen
{
namespace cuda
{
	
error::error( const std::string& msg) : std::runtime_error( msg)
{
}

} // namespace
} // namespace
