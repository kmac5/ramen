// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/imageio/jpg/jpg_writer.hpp>

#include<stdio.h>

extern "C"
{
#include <jpeglib.h>
}

namespace ramen
{
namespace imageio
{
namespace
{

struct FILE_deleter
{
    void operator()( FILE *x) const
    {
        if( x)
            fclose( x);
    }
};

} // unamed

void jpg_writer_t::do_write_image( const boost::filesystem::path& p,
                                    const image::const_image_view_t& view,
                                    const core::dictionary_t& params) const
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    boost::shared_ptr<FILE> fp( fopen( filesystem::file_cstring( p), "wb"),
                                 FILE_deleter());

    if( !fp)
		throw exception( "Can't open file");

    cinfo.err = jpeg_std_error( &jerr);
    jpeg_create_compress( &cinfo);
    jpeg_stdio_dest( &cinfo, fp.get());
    cinfo.image_width = view.width();
    cinfo.image_height = view.height();
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;
    jpeg_set_defaults( &cinfo);
    jpeg_set_quality( &cinfo,
                      core::get<int>( params, core::name_t( "quality")),
                      TRUE);

    jpeg_start_compress( &cinfo, TRUE);

    std::vector<boost::uint8_t> buffer( view.width() * 3);

    for( int y = 0; y < view.height(); ++y)
    {
		boost::uint8_t *ptr = &buffer.front();
		image::const_image_view_t::x_iterator src_it( view.row_begin( y));

		for( int x = 0; x < view.width(); ++x)
		{
		    *ptr++ = convert8( boost::gil::get_color( *src_it, boost::gil::red_t()));
		    *ptr++ = convert8( boost::gil::get_color( *src_it, boost::gil::green_t()));
		    *ptr++ = convert8( boost::gil::get_color( *src_it, boost::gil::blue_t()));
		    ++src_it;
		}

		ptr = &buffer.front();
		jpeg_write_scanlines( &cinfo, &ptr, 1);
    }

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
}
    
} // imageio
} // ramen
