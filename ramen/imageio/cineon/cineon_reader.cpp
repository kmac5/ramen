// Copyright (c) 2010 Esteban Tovagliari.

#include<ramen/imageio/cineon/cineon_reader.hpp>

#include<IECore/ByteOrder.h>

#include"cineon_private.hpp"

namespace ramen
{
namespace imageio
{

cineon_reader_t::cineon_reader_t( const boost::filesystem::path& p) : reader_t( p)
{
    boost::filesystem::ifstream in( p);

    if ( !in.is_open() || in.fail())
		throw exception( "Can't open file");

    read_header( in);
    info_[adobe::name_t( "format")] = adobe::any_regular_t( Imath::Box2i( Imath::V2i( 0, 0), Imath::V2i( width_ - 1, height_ - 1)));
}

void cineon_reader_t::do_read_image( const image::image_view_t& view, const Imath::Box2i& crop, int subsample) const
{
    boost::filesystem::ifstream in( path_, std::ios_base::binary);

    if ( !in.is_open() || in.fail())
		throw exception( "Can't open file");

    read_header( in);
    in.clear();
    in.seekg( data_offset_ + ( crop.min.y * width_ * sizeof( boost::uint32_t)), std::ios_base::beg);

    if( in.fail())
		throw exception( "Error reading image");

    std::vector<boost::uint32_t> buffer( width_);

    int y = crop.min.y;
    int yy = 0;

    while( y <= crop.max.y)
    {
		in.read( reinterpret_cast<char*>( &buffer[0]), sizeof( boost::uint32_t) * width_);

		if( in.fail())
		    throw exception( "cineon reader: error while reading image\n");

		image::image_view_t::x_iterator dst_it( view.row_begin( yy));
		image::image_view_t::x_iterator dst_end( view.row_end( yy));
		boost::uint32_t *q = &( *buffer.begin());

		for( int x = crop.min.x; x <= crop.max.x; x += subsample)
		{
			boost::uint32_t pix = q[x];
	
			if( reverse_bytes_)
				pix = IECore::reverseBytes( pix);
	
			boost::gil::get_color( *dst_it, boost::gil::red_t())    = (( pix >> 22) & 0x3ff) / 1023.0f;
			boost::gil::get_color( *dst_it, boost::gil::green_t())  = (( pix >> 12) & 0x3ff) / 1023.0f;
			boost::gil::get_color( *dst_it, boost::gil::blue_t())   = (( pix >> 02) & 0x3ff) / 1023.0f;
			boost::gil::get_color( *dst_it, boost::gil::alpha_t())  = 1.0f;
			++dst_it;
	
			if( dst_it == dst_end)
			break;
		}

        // fill the missing pixels with the last one
        while( dst_it < dst_end)
        {
		    boost::uint32_t pix = buffer.back();

		    if( reverse_bytes_)
				pix = IECore::reverseBytes( pix);

		    boost::gil::get_color( *dst_it, boost::gil::red_t())    = (( pix >> 22) & 0x3ff) / 1023.0f;
		    boost::gil::get_color( *dst_it, boost::gil::green_t())  = (( pix >> 12) & 0x3ff) / 1023.0f;
		    boost::gil::get_color( *dst_it, boost::gil::blue_t())   = (( pix >> 02) & 0x3ff) / 1023.0f;
		    boost::gil::get_color( *dst_it, boost::gil::alpha_t())  = 1.0f;
		    ++dst_it;
        }

		++yy;
		++y;

		// skip subsample - 1 scanlines
		for( int s = 0; s < subsample-1; ++s)
		{
			if( y <= crop.max.y)
			{
			    in.read( reinterpret_cast<char*>( &buffer[0]), sizeof( boost::uint32_t) * width_);

			    if( in.fail())
					throw exception( "cineon reader: error while reading image\n");
			}

		    ++y;
		}
    }

    repeat_scanline_until_end( view, yy - 1);
}

void cineon_reader_t::read_header( boost::filesystem::ifstream& in) const
{
    CineonFileInformation file_info;

    in.read( reinterpret_cast<char*>( &file_info), sizeof( file_info));

    if( in.fail())
		throw unknown_image_format();

    reverse_bytes_ = false;

    if ( file_info.magic == 0xd75f2a80 )
	    reverse_bytes_ = true;
    else
    {
		if( file_info.magic != 0x802a5fd7 )
		    throw unknown_image_format();
    }

    CineonImageInformation image_info;

    in.read( reinterpret_cast<char*>( &image_info), sizeof( image_info));
    if( in.fail())
		throw unknown_image_format();
	
    CineonImageDataFormatInformation data_format_info;

    in.read( reinterpret_cast<char*>( &data_format_info), sizeof( data_format_info));
    if ( in.fail())
		throw unknown_image_format();
	
    if ( (int)data_format_info.packing != 5 )
		throw unsupported_image( "Unsupported data packing in file");

    if ( (int)data_format_info.interleave != 0 )
	    throw unsupported_image( "Unsupported data interleaving in file");

    if ( (int)data_format_info.data_signed != 0 )
	    throw unsupported_image( "Unsupported data signing in file");

    if ( (int)data_format_info.sense != 0 )
	    throw unsupported_image( "Unsupported data sense in file");

    if ( (int)data_format_info.eol_padding != 0 || (int)data_format_info.eoc_padding != 0 )
	    throw unsupported_image( "Unsupported data padding in file ");

    CineonImageOriginationInformation origin_info;

    in.read( reinterpret_cast<char*>( &origin_info), sizeof(origin_info));
    if( in.fail())
		throw unknown_image_format();

    if ( reverse_bytes_)
    {
		file_info.image_data_offset         = IECore::reverseBytes( file_info.image_data_offset);
		file_info.industry_header_length    = IECore::reverseBytes( file_info.industry_header_length);
		file_info.variable_header_length    = IECore::reverseBytes( file_info.variable_header_length);
    }
	
    if ( (int)image_info.orientation != 0 )
		throw unsupported_image( "Unsupported image orientation in file");
	
    if( image_info.channel_count != 3)
		throw unsupported_image( "Unsupported number of channels in file");
	
    for ( int i = 0; i < (int)image_info.channel_count; ++i )
    {
	    CineonImageInformationChannelInformation& channelInformation = image_info.channel_information[i];
		
	    if ( reverse_bytes_)
	    {
			channelInformation.pixels_per_line = IECore::reverseBytes( channelInformation.pixels_per_line );
			channelInformation.lines_per_image = IECore::reverseBytes( channelInformation.lines_per_image );
	    }
		
	    if( i == 0)
	    {
			width_  = channelInformation.pixels_per_line;
			height_ = channelInformation.lines_per_image;
	    }
	    else
	    {
			if ( channelInformation.pixels_per_line != width_ || channelInformation.lines_per_image != height_)
				throw unsupported_image( "Cannot read channels of differing dimensions in file");
	    }
		
	    if ( (int)channelInformation.bpp != 10 )
			throw unsupported_image( "Unsupported bits per pixel in file");
		
	    if ( channelInformation.byte_0 == 1 )
			throw unsupported_image( "Cannot read vendor specific Cineon file ");
    }
	
    data_offset_ = file_info.image_data_offset;
}

} // namespace
} // namespace
