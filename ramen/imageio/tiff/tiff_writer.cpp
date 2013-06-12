// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/imageio/tiff/tiff_writer.hpp>

#include<memory>
#include<vector>
#include<algorithm>

#include<ramen/imageio/enums.hpp>

namespace ramen
{
namespace imageio
{

void tiff_writer_t::do_write_image( const boost::filesystem::path& p,
                                    const image::const_image_view_t& view,
                                    const core::dictionary_t& params) const
{
    int channels    = core::get<int>( params, core::name_t( "channels"));
    int type	    = core::get<int>( params, core::name_t( "type"));
    int compress    = core::get<int>( params, core::name_t( "compress"));

    std::auto_ptr<OIIO::ImageOutput> out( OIIO::ImageOutput::create( filesystem::file_string( p)));

    if( !out.get())
		throw exception( "Write TIFF: Can't open output file");

    if( channels)
		channels = 3;
    else
		channels = 4;

    OIIO::ImageSpec spec;

    switch( type)
    {
		case ubyte_channel_type:
            spec = OIIO::ImageSpec( view.width(), view.height(),
                                    channels, OIIO::TypeDesc::UINT8);
		break;
	
		case ushort_channel_type:
            spec = OIIO::ImageSpec( view.width(), view.height(),
                                    channels, OIIO::TypeDesc::UINT16);
		break;
	
		case float_channel_type:
            spec = OIIO::ImageSpec( view.width(), view.height(),
                                    channels, OIIO::TypeDesc::FLOAT);
		break;
		
		default:
			throw unsupported_image( "unsupported depth");
    }

    switch( compress)
    {
		case none_compression:
			spec.attribute( "compress", "none");
		break;
	
		case lzw_compression:
			spec.attribute( "compress", "lzw");
		break;
	
		case zip_compression:
			spec.attribute( "compress", "zip");
		break;

		default:
			throw unsupported_image( "unsupported compression");
    }

	add_common_attributes( spec);
	
    if( !out->open( filesystem::file_string( p), spec))
		throw exception( "Can't open output file");

    std::vector<image::pixel_t> scanline( view.width());

    for( int y = 0; y < view.height(); ++y)
    {
		std::copy( view.row_begin( y), view.row_end( y), scanline.begin());

		if( type != float_channel_type)
		    clamp( scanline.begin(), scanline.end());

		if( !out->write_scanline( y, 0, OIIO::TypeDesc::FLOAT, (void *) &( *scanline.begin()), sizeof( image::pixel_t)))
		    throw exception( "Write image: Can't write pixels");
    }

    if( !out->close())
		throw exception( "Write image: Can't close file");
}
    
} // imageio
} // ramen
