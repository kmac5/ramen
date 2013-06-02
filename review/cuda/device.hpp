// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_CUDA_DEVICE_HPP
#define RAMEN_CUDA_DEVICE_HPP

#include<string>

#include<ramen/cuda/cuda.hpp>
#include<ramen/cuda/cudart.hpp>

namespace ramen
{
namespace cuda
{

class device_t
{
public:

	explicit device_t( int index);

	std::string name() const { return properties.name;}

	int capability_major() const { return properties.major;}
	int capability_minor() const { return properties.minor;}

	int warp_size() const { return properties.warpSize;}

	std::size_t global_memory() const { return properties.totalGlobalMem;}

	CUdevice get_cu_device() const;

	std::string description() const;

	int device_index;
    struct cudaDeviceProp properties;

private:

	int convert_sm_ver2_cores( int major, int minor) const;
};

} // namespace
} // namespace

#endif
