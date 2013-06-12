// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/imageio/exr/exr_writer.hpp>

#include<gil/extension/half.hpp>

#include<OpenEXR/ImfOutputFile.h>
#include<OpenEXR/ImfChannelList.h>

#include<ramen/assert.hpp>

namespace ramen
{
namespace imageio
{
namespace
{

Imath::Box2i convert_box( const math::box2i_t& box)
{
    return Imath::Box2i( Imath::V2i( box.min.x, box.min.y),
                         Imath::V2i( box.max.x, box.max.y));
}

} // unnamed

void exr_writer_t::do_write_image( const boost::filesystem::path& p,
				const image::const_image_view_t& view,
                const core::dictionary_t& params) const
{
    int channels    = core::get<int>( params, core::name_t( "channels"));
    int type	    = core::get<int>( params, core::name_t( "type"));
    int compress    = core::get<int>( params, core::name_t( "compress"));
    float aspect	= core::get<float>( params, core::name_t( "aspect"));

    Imath::Box2i display_window = convert_box( core::get<math::box2i_t>( params,
                                                                         core::name_t( "format")));
    Imath::Box2i data_window = display_window;
    {
        const math::box2i_t *tmp = core::get<math::box2i_t>( &params,
                                                             core::name_t( "bounds"));

        if( tmp)
            data_window = convert_box( *tmp);
    }

	RAMEN_ASSERT( display_window.intersects( data_window));

	Imf::Header header( display_window, data_window, aspect);

    switch( compress)
    {
		case none_compression:
			header.compression() = Imf::NO_COMPRESSION;
		break;
	
		case rle_compression:
			header.compression() = Imf::RLE_COMPRESSION;
		break;
	
		case zips_compression:
			header.compression() = Imf::ZIPS_COMPRESSION;
		break;
	
		case zip_compression:
			header.compression() = Imf::ZIP_COMPRESSION;
		break;
	
		case piz_compression:
			header.compression() = Imf::PIZ_COMPRESSION;
		break;
	
		case pxr24_compression:
			header.compression() = Imf::PXR24_COMPRESSION;
		break;
	
		case b44_compression:
			header.compression() = Imf::B44_COMPRESSION;
		break;
	
		case b44a_compression:
			header.compression() = Imf::B44A_COMPRESSION;
		break;
		
		default:
			throw unsupported_image( "compression method not supported");
    }

	// handle more attributes here...

	image::const_image_view_t subview( boost::gil::subimage_view( view, data_window.min.x - display_window.min.x,
																		data_window.min.y - display_window.min.y,
																		data_window.size().x + 1, data_window.size().y + 1));
	
	switch( type)
	{
		case half_channel_type:
			if( channels == alpha_channel)
				write_half_alpha( p, header, subview);
			else
				write_half( p, header, subview, channels == rgba_channels);
		break;
		
		case float_channel_type:
			if( channels == alpha_channel)
				write_float_alpha( p, header, subview);
			else
				write_float( p, header, subview, channels == rgba_channels);
		break;
		
		default:
			throw unsupported_image( "depth not supported");
	}
}

void exr_writer_t::write_half( const boost::filesystem::path& p,
                               Imf::Header& header,
                               const image::const_image_view_t& view,
                               bool save_alpha) const
{
    boost::gil::rgba16f_image_t img( view.width(), view.height());
    boost::gil::copy_and_convert_pixels( view, boost::gil::view( img));

    header.channels().insert( "R", Imf::HALF);
    header.channels().insert( "G", Imf::HALF);
    header.channels().insert( "B", Imf::HALF);

    if( save_alpha)
        header.channels().insert( "A", Imf::HALF);

    Imf::FrameBuffer frameBuffer;

    char *ptr = (char *) boost::gil::interleaved_view_get_raw_data( boost::gil::view( img));
    std::size_t xstride = 4 * sizeof(half);
    std::size_t ystride = xstride * img.width();
	ptr = ptr - ( header.dataWindow().min.y * ystride) - ( header.dataWindow().min.x * xstride);
	
    frameBuffer.insert( "R", Imf::Slice( Imf::HALF, ptr, xstride, ystride)); ptr += sizeof(half);
    frameBuffer.insert( "G", Imf::Slice( Imf::HALF, ptr, xstride, ystride)); ptr += sizeof(half);
    frameBuffer.insert( "B", Imf::Slice( Imf::HALF, ptr, xstride, ystride)); ptr += sizeof(half);

    if( save_alpha)
        frameBuffer.insert( "A", Imf::Slice( Imf::HALF, ptr, xstride, ystride));

    Imf::OutputFile out_file( filesystem::file_cstring( p), header);
    out_file.setFrameBuffer( frameBuffer);
    out_file.writePixels( img.height());
}

void exr_writer_t::write_half_alpha( const boost::filesystem::path& p,
                                     Imf::Header& header,
                                     const image::const_image_view_t& view) const
{
    boost::gil::gray16f_image_t img( view.width(), view.height());
    boost::gil::copy_and_convert_pixels( boost::gil::nth_channel_view( view, 3), boost::gil::view( img));

    header.channels().insert( "A", Imf::HALF);
    Imf::FrameBuffer frameBuffer;

    char *ptr = (char *) boost::gil::interleaved_view_get_raw_data( boost::gil::view( img));
    std::size_t xstride = sizeof(half);
    std::size_t ystride = xstride * img.width();
	ptr = ptr - ( header.dataWindow().min.y * ystride) - ( header.dataWindow().min.x * xstride);

    frameBuffer.insert( "A", Imf::Slice( Imf::HALF, ptr, xstride, ystride));

    Imf::OutputFile out_file( filesystem::file_cstring( p), header);
    out_file.setFrameBuffer( frameBuffer);
    out_file.writePixels( img.height());
}

void exr_writer_t::write_float( const boost::filesystem::path& p,
                                Imf::Header& header,
                                const image::const_image_view_t& view,
                                bool save_alpha) const
{
    header.channels().insert( "R", Imf::FLOAT);
    header.channels().insert( "G", Imf::FLOAT);
    header.channels().insert( "B", Imf::FLOAT);

    if( save_alpha)
        header.channels().insert( "A", Imf::FLOAT);

    Imf::FrameBuffer frameBuffer;

    char *ptr = (char *) boost::gil::interleaved_view_get_raw_data( view);
    std::size_t xstride = 4 * sizeof( float);
    std::size_t ystride = xstride * view.width();
	ptr = ptr - ( header.dataWindow().min.y * ystride) - ( header.dataWindow().min.x * xstride);

    frameBuffer.insert( "R", Imf::Slice( Imf::FLOAT, ptr, xstride, ystride)); ptr += sizeof(float);
    frameBuffer.insert( "G", Imf::Slice( Imf::FLOAT, ptr, xstride, ystride)); ptr += sizeof(float);
    frameBuffer.insert( "B", Imf::Slice( Imf::FLOAT, ptr, xstride, ystride)); ptr += sizeof(float);

    if( save_alpha)
        frameBuffer.insert( "A", Imf::Slice( Imf::FLOAT, ptr, xstride, ystride));

    Imf::OutputFile out_file( filesystem::file_cstring( p), header);
    out_file.setFrameBuffer( frameBuffer);
    out_file.writePixels( view.height());
}

void exr_writer_t::write_float_alpha( const boost::filesystem::path& p,
                                      Imf::Header& header,
                                      const image::const_image_view_t& view) const
{
    header.channels().insert( "A", Imf::FLOAT);

    Imf::FrameBuffer frameBuffer;

    char *ptr = (char *) boost::gil::interleaved_view_get_raw_data( view);
    ptr += 3 * sizeof( float);
    std::size_t xstride = 4 * sizeof( float);
    std::size_t ystride = xstride * view.width();
	ptr = ptr - ( header.dataWindow().min.y * ystride) - ( header.dataWindow().min.x * xstride);

    frameBuffer.insert( "A", Imf::Slice( Imf::FLOAT, ptr, xstride, ystride));

    Imf::OutputFile out_file( filesystem::file_cstring( p), header);
    out_file.setFrameBuffer( frameBuffer);
    out_file.writePixels( view.height());
}

} // imageio
} // ramen
