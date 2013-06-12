// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/imageio/hdr/hdr_writer.hpp>

namespace ramen
{
namespace imageio
{

void hdr_writer_t::do_write_image( const boost::filesystem::path& p,
				const image::const_image_view_t& view,
                const core::dictionary_t& params) const
{
    std::auto_ptr<OIIO::ImageOutput> out( OIIO::ImageOutput::create( filesystem::file_string( p)));

    if( !out.get())
		throw exception( "Write HDR: Can't open output file");

    OIIO::ImageSpec spec( view.width(), view.height(), 3, OIIO::TypeDesc::FLOAT);
	add_common_attributes( spec);

    if( !out->open( filesystem::file_string( p), spec))
		throw exception( "Write HDR: Can't open output file");

    char *pixels = (char *) boost::gil::interleaved_view_get_raw_data( view);
    std::size_t xstride = 4 * sizeof( float);
    std::size_t ystride = xstride * view.width();

    if( !out->write_image( OIIO::TypeDesc::FLOAT, pixels, xstride, ystride, OIIO::AutoStride))
		throw exception( "Write HDR: Can't write pixels");

    if( !out->close())
		throw exception( "Write HDR: Can't close file");
}

} // imageio
} // ramen
