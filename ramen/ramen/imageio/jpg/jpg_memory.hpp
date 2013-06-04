// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_IMAGEIO_JPEG_MEMORY_HPP
#define RAMEN_IMAGEIO_JPEG_MEMORY_HPP

#include<ramen/imageio/memory.hpp>

extern "C"
{
#include <jpeglib.h>
}

namespace adobe
{

template<>
struct delete_ptr_trait<jpeg_decompress_struct*>
{
    void operator()( jpeg_decompress_struct *cinfo) const
    {
        jpeg_finish_decompress( cinfo);
        jpeg_destroy_decompress( cinfo);
    }
};

template<>
struct delete_ptr_trait<jpeg_compress_struct*>
{
    void operator()( jpeg_compress_struct *cinfo) const
    {
        jpeg_finish_compress( cinfo);
        jpeg_destroy_compress( cinfo);
    }
};

} // namespace

#endif
