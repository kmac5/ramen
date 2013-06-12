// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/imageio/oiio/oiio_writer.hpp>

#include<memory>

#include<OpenImageIO/typedesc.h>

namespace ramen
{
namespace imageio
{

void oiio_writer_t::do_write( const boost::filesystem::path& p,
                              const OIIO::ImageSpec& spec,
                              const image::const_image_view_t& view)
{
    std::auto_ptr<OIIO::ImageOutput> out( OIIO::ImageOutput::create( filesystem::file_string( p)));

    if( !out.get())
		throw exception( "OIIO, write image: Can't find a format");

    if( !out->open( filesystem::file_string( p), spec))
		throw exception( "OIIO, write image: Can't open file");

    char *pixels = (char *) boost::gil::interleaved_view_get_raw_data( view);
    
    if( spec.nchannels == 1) // save alpha channel
		pixels += 3;

    std::size_t xstride = 4 * sizeof( float);
    std::size_t ystride = xstride * view.width();

    if( !out->write_image( OIIO::TypeDesc::FLOAT, pixels, xstride, ystride, OIIO::AutoStride))
		throw( exception( "Write image: Can't write pixels"));

    if( !out->close())
		throw exception( "Write image: Can't close file");
}

void oiio_writer_t::add_common_attributes( OIIO::ImageSpec& spec) const
{
	spec.attribute( "oiio:UnassociatedAlpha", ( int) 0);	
}

} // imageio
} // ramen
