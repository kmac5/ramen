// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/cuda/delay_load_libcuda.hpp>

#include<dlfcn.h>

#include<ramen/assert.hpp>

namespace ramen
{
namespace cuda
{
namespace
{

void *cuda_lib_ = 0;

} // unnamed

bool do_load_libcuda()
{
	cuda_lib_ = dlopen( "libcuda.so", RTLD_NOW);

    if( !cuda_lib_)
		return false;

	return true;
}

void *get_proc_ex( const std::string& name, bool required)
{
	RAMEN_ASSERT( cuda_lib_);

	void *fun = dlsym( cuda_lib_, name.c_str());
	return fun;
}

void *get_proc_ex_v2( const std::string& name, bool required)
{
	RAMEN_ASSERT( cuda_lib_);

	std::string name_v2 = name + std::string( "_v2");
	void *fun = dlsym( cuda_lib_, name_v2.c_str());
	return fun;
}

void do_unload_libcuda()
{
	if( cuda_lib_)
	{
		dlclose( cuda_lib_);
		cuda_lib_ = 0;
	}
}

} // namespace
} // namespace


