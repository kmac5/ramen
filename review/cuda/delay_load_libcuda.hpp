// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_CUDA_DELAY_LOAD_LIBCUDA_HPP
#define RAMEN_CUDA_DELAY_LOAD_LIBCUDA_HPP

#include<string>

namespace ramen
{
namespace cuda
{

bool do_load_libcuda();

void *get_proc_ex( const std::string& name, bool required = true);
void *get_proc_ex_v2( const std::string& name, bool required = true);

void do_unload_libcuda();

} // namespace
} // namespace

#endif
