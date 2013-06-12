// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/imageio/tga/tga_writer.hpp>

#include<vector>
#include<algorithm>

namespace ramen
{
namespace imageio
{

void tga_writer_t::do_write_image( const boost::filesystem::path& p,
                                    const image::const_image_view_t& view,
                                    const core::dictionary_t& params) const
{
    int channels    = core::get<int>( params, core::name_t( "channels"));
    int compress    = core::get<int>( params, core::name_t( "compress"));
    
    std::auto_ptr<OIIO::ImageOutput> out( OIIO::ImageOutput::create( filesystem::file_string( p)));

    if( !out.get())
		throw exception( "Write TGA: Can't open output file");

    if( channels)
		channels = 3;
    else
		channels = 4;

    OIIO::ImageSpec spec( view.width(), view.height(), channels, OIIO::TypeDesc::UINT8);
	add_common_attributes( spec);

	switch( compress)
	{
		case none_compression:
		break;
		
		case rle_compression:
			spec.attribute( "compression", "rle");
		break;
		
		default:
			throw unsupported_image( "unsupported compression");
	}

    if( !out->open( filesystem::file_string( p), spec))
		throw exception( "Can't open output file");

    std::vector<image::pixel_t> scanline( view.width());

    for( int y = 0; y < view.height(); ++y)
    {
		std::copy( view.row_begin( y), view.row_end( y), scanline.begin());
		clamp( scanline.begin(), scanline.end());
	
		if( !out->write_scanline( y, 0, OIIO::TypeDesc::FLOAT, (void *) &( *scanline.begin()), sizeof( image::pixel_t)))
			throw exception( "Write image: Can't write pixels");
    }

    if( !out->close())
		throw exception( "Write image: Can't close file");
}

} // imageio
} // ramen
