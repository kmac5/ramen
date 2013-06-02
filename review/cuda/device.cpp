// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/cuda/device.hpp>

#include<sstream>

namespace ramen
{
namespace cuda
{

device_t::device_t( int index) : device_index( index)
{
	cudaGetDeviceProperties( &properties, device_index);
}

CUdevice device_t::get_cu_device() const
{
	CUdevice result;
	cu_device_get( &result, device_index);
	return result;
}

std::string device_t::description() const
{
	std::stringstream s;
	s << properties.name << "\n";
	s << "Ram = " << properties.totalGlobalMem / 1024 / 1024 << " Mbs.\n";
	s << "CUDA Capability Major/Minor: " << properties.major << ", " << properties.minor << "\n";

	s << properties.multiProcessorCount << " Multiprocessors x " <<
		 convert_sm_ver2_cores( properties.major, properties.minor) * properties.multiProcessorCount <<  " CUDA cores.\n";

	s << "GPU Clock Speed: " << properties.clockRate * 1e-6f << " GHz\n";
	return s.str();
}

int device_t::convert_sm_ver2_cores( int major, int minor) const
{
	typedef struct
	{
		int SM; // 0xMm (hexidecimal notation), M = SM Major version, and m = SM minor version
		int Cores;
	} s_sm_to_cores;

	s_sm_to_cores n_gpu_arch_cores_per_sm[] =
	{ { 0x10,  8 },
	  { 0x11,  8 },
	  { 0x12,  8 },
	  { 0x13,  8 },
	  { 0x20, 32 },
	  { 0x21, 48 },
	  {   -1, -1 }
	};

	int index = 0;

	while( n_gpu_arch_cores_per_sm[index].SM != -1)
	{
		if( n_gpu_arch_cores_per_sm[index].SM == ((major << 4) + minor))
			return n_gpu_arch_cores_per_sm[index].Cores;

		index++;
	}

	return -1;
}

} // namespace
} // namespace

/*
    for (dev = 0; dev < deviceCount; ++dev) {
        cudaDeviceProp deviceProp;
        cudaGetDeviceProperties(&deviceProp, dev);

        shrLog("\nDevice %d: \"%s\"\n", dev, deviceProp.name);

    #if CUDART_VERSION >= 2020
        // Console log
        cudaDriverGetVersion(&driverVersion);
        cudaRuntimeGetVersion(&runtimeVersion);
        shrLog("  CUDA Driver Version / Runtime Version          %d.%d / %d.%d\n", driverVersion/1000, driverVersion%100, runtimeVersion/1000, runtimeVersion%100);
    #endif
        shrLog("  CUDA Capability Major/Minor version number:    %d.%d\n", deviceProp.major, deviceProp.minor);

        char msg[256];
        sprintf(msg, "  Total amount of global memory:                 %.0f MBytes (%llu bytes)\n",
                      (float)deviceProp.totalGlobalMem/1048576.0f, (unsigned long long) deviceProp.totalGlobalMem);
        shrLog(msg);
    #if CUDART_VERSION >= 2000
        shrLog("  (%2d) Multiprocessors x (%2d) CUDA Cores/MP:     %d CUDA Cores\n",
			deviceProp.multiProcessorCount,
			ConvertSMVer2Cores(deviceProp.major, deviceProp.minor),
			ConvertSMVer2Cores(deviceProp.major, deviceProp.minor) * deviceProp.multiProcessorCount);
    #endif
        shrLog("  GPU Clock Speed:                               %.2f GHz\n", deviceProp.clockRate * 1e-6f);
    #if CUDART_VERSION >= 4000
	// This is not available in the CUDA Runtime API, so we make the necessary calls the driver API to support this for output
        int memoryClock;
        getCudaAttribute<int>( &memoryClock, CU_DEVICE_ATTRIBUTE_MEMORY_CLOCK_RATE, dev );
        shrLog("  Memory Clock rate:                             %.2f Mhz\n", memoryClock * 1e-3f);
        int memBusWidth;
        getCudaAttribute<int>( &memBusWidth, CU_DEVICE_ATTRIBUTE_GLOBAL_MEMORY_BUS_WIDTH, dev );
        shrLog("  Memory Bus Width:                              %d-bit\n", memBusWidth);
        int L2CacheSize;
        getCudaAttribute<int>( &L2CacheSize, CU_DEVICE_ATTRIBUTE_L2_CACHE_SIZE, dev );
        if (L2CacheSize) {
            shrLog("  L2 Cache Size:                                 %d bytes\n", L2CacheSize);
        }

        shrLog("  Max Texture Dimension Size (x,y,z)             1D=(%d), 2D=(%d,%d), 3D=(%d,%d,%d)\n",
                                                        deviceProp.maxTexture1D, deviceProp.maxTexture2D[0], deviceProp.maxTexture2D[1],
                                                        deviceProp.maxTexture3D[0], deviceProp.maxTexture3D[1], deviceProp.maxTexture3D[2]);
        shrLog("  Max Layered Texture Size (dim) x layers        1D=(%d) x %d, 2D=(%d,%d) x %d\n",
                                                        deviceProp.maxTexture1DLayered[0], deviceProp.maxTexture1DLayered[1],
                                                        deviceProp.maxTexture2DLayered[0], deviceProp.maxTexture2DLayered[1], deviceProp.maxTexture2DLayered[2]);
    #endif
        shrLog("  Total amount of constant memory:               %u bytes\n", deviceProp.totalConstMem);
        shrLog("  Total amount of shared memory per block:       %u bytes\n", deviceProp.sharedMemPerBlock);
        shrLog("  Total number of registers available per block: %d\n", deviceProp.regsPerBlock);
        shrLog("  Warp size:                                     %d\n", deviceProp.warpSize);
        shrLog("  Maximum number of threads per block:           %d\n", deviceProp.maxThreadsPerBlock);
        shrLog("  Maximum sizes of each dimension of a block:    %d x %d x %d\n",
               deviceProp.maxThreadsDim[0],
               deviceProp.maxThreadsDim[1],
               deviceProp.maxThreadsDim[2]);
        shrLog("  Maximum sizes of each dimension of a grid:     %d x %d x %d\n",
               deviceProp.maxGridSize[0],
               deviceProp.maxGridSize[1],
               deviceProp.maxGridSize[2]);
        shrLog("  Maximum memory pitch:                          %u bytes\n", deviceProp.memPitch);
		shrLog("  Texture alignment:                             %u bytes\n", deviceProp.textureAlignment);

    #if CUDART_VERSION >= 4000
        shrLog("  Concurrent copy and execution:                 %s with %d copy engine(s)\n", (deviceProp.deviceOverlap ? "Yes" : "No"), deviceProp.asyncEngineCount);
    #else
        shrLog("  Concurrent copy and execution:                 %s\n", deviceProp.deviceOverlap ? "Yes" : "No");
    #endif

    #if CUDART_VERSION >= 2020
        shrLog("  Run time limit on kernels:                     %s\n", deviceProp.kernelExecTimeoutEnabled ? "Yes" : "No");
        shrLog("  Integrated GPU sharing Host Memory:            %s\n", deviceProp.integrated ? "Yes" : "No");
        shrLog("  Support host page-locked memory mapping:       %s\n", deviceProp.canMapHostMemory ? "Yes" : "No");
    #endif
    #if CUDART_VERSION >= 3000
        shrLog("  Concurrent kernel execution:                   %s\n", deviceProp.concurrentKernels ? "Yes" : "No");
        shrLog("  Alignment requirement for Surfaces:            %s\n", deviceProp.surfaceAlignment ? "Yes" : "No");
    #endif
    #if CUDART_VERSION >= 3010
        shrLog("  Device has ECC support enabled:                %s\n", deviceProp.ECCEnabled ? "Yes" : "No");
    #endif
    #if CUDART_VERSION >= 3020
        shrLog("  Device is using TCC driver mode:               %s\n", deviceProp.tccDriver ? "Yes" : "No");
    #endif
    #if CUDART_VERSION >= 4000
        shrLog("  Device supports Unified Addressing (UVA):      %s\n", deviceProp.unifiedAddressing ? "Yes" : "No");
        shrLog("  Device PCI Bus ID / PCI location ID:           %d / %d\n", deviceProp.pciBusID, deviceProp.pciDeviceID );
    #endif

    #if CUDART_VERSION >= 2020
        const char *sComputeMode[] = {
            "Default (multiple host threads can use ::cudaSetDevice() with device simultaneously)",
            "Exclusive (only one host thread in one process is able to use ::cudaSetDevice() with this device)",
            "Prohibited (no host thread can use ::cudaSetDevice() with this device)",
            "Exclusive Process (many threads in one process is able to use ::cudaSetDevice() with this device)",
            "Unknown",
            NULL
        };
        shrLog("  Compute Mode:\n");
        shrLog("     < %s >\n", sComputeMode[deviceProp.computeMode]);
    #endif
    }
*/
