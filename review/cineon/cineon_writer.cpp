// Copyright (c) 2010 Esteban Tovagliari.

#include<ramen/imageio/cineon/cineon_writer.hpp>

#include<cstdio>

#include<fstream>

#include<boost/date_time/posix_time/ptime.hpp>
#include<boost/date_time/posix_time/posix_time_types.hpp>
using namespace boost;

#include<IECore/ByteOrder.h>

#include"cineon_private.hpp"

namespace ramen
{
namespace imageio
{

void cineon_writer_t::do_write_image( const boost::filesystem::path& p,
				const image::const_image_view_t& view,
				const adobe::dictionary_t& params) const
{
    std::ofstream out( filesystem::file_cstring( p), std::ios_base::binary);

    if( !out)
		throw exception( "can't open file");

    CineonFileInformation fi = { 0 };

    fi.magic = IECore::asBigEndian( (boost::uint32_t) 0x802a5fd7);
    fi.section_header_length = 0;
    fi.industry_header_length = 0;
    fi.variable_header_length = 0;

    strcpy( fi.version, "V4.5");

    strncpy( (char *) fi.file_name, filesystem::file_cstring( p), sizeof( fi.file_name ));

    // compute the current date and time
    boost::posix_time::ptime localTime = boost::posix_time::second_clock::local_time();
    boost::gregorian::date date = localTime.date();
    boost::posix_time::time_duration time = localTime.time_of_day();

    #if defined(WIN32) || defined(WIN64)
        sprintf_s( fi.creation_date, sizeof( fi.creation_date ), "%04d-%02d-%02d",
                static_cast<int>( date.year()), static_cast<int>( date.month()),
                static_cast<int>( date.day()));

        sprintf_s( fi.creation_time, sizeof( fi.creation_time ), "%02d:%02d:%02d", time.hours(),
                    time.minutes(), time.seconds());
    #else
        snprintf( fi.creation_date, sizeof( fi.creation_date ), "%04d-%02d-%02d",
                static_cast<int>( date.year()), static_cast<int>( date.month()),
                static_cast<int>( date.day()));

        snprintf( fi.creation_time, sizeof( fi.creation_time ), "%02d:%02d:%02d", time.hours(),
                    time.minutes(), time.seconds());
    #endif

    CineonImageInformation ii = { 0 };
    ii.orientation = 0;
    ii.channel_count = 3;

    for( int c = 0; c < 8; ++c)
    {
		CineonImageInformationChannelInformation& ci = ii.channel_information[c];
	
		ci.byte_0 = 0;
	
		if( c == 0)
			ci.byte_1 = 1;
	
		if( c == 1)
			ci.byte_1 = 2;
	
		if( c == 2)
			ci.byte_1 = 3;
	
	
		ci.byte_1 = 0;
		ci.bpp = 10;
	
		if( c < 3)
		{
			ci.pixels_per_line = IECore::asBigEndian( (boost::uint32_t) view.width());
			ci.lines_per_image = IECore::asBigEndian( (boost::uint32_t) view.height());
	
			ci.min_data_value	= IECore::asBigEndian( 0.0f);
			ci.min_quantity	= IECore::asBigEndian( 0.0f);
			ci.max_data_value	= IECore::asBigEndian( 1023.0f);
			ci.max_quantity	= IECore::asBigEndian( 2.046f);
		}
		else
		{
			ci.pixels_per_line = 0;
			ci.lines_per_image = 0;
		}
    }

    CineonImageDataFormatInformation idfi = { 0 };
    idfi.interleave = 0;               // pixel interleave
    idfi.packing = 5;                  // 32 bit left-aligned with 2 waste bits
    idfi.data_signed = 0;              // unsigned data
    idfi.sense = 0;                    // positive image sense
    idfi.eol_padding = 0;              // no end-of-line padding
    idfi.eoc_padding = 0;              // no end-of-data padding

    // \todo Complete filling in this structure
    CineonImageOriginationInformation ioi = { 0 };
    ioi.x_offset = 0;                  // could be dataWindow min.x
    ioi.y_offset = 0;                  // could be dataWindow min.y
    ioi.gamma = IECore::asBigEndian( (boost::uint32_t) 0x7f800000);

    // compute data offsets
    fi.image_data_offset = IECore::asBigEndian( (boost::uint32_t) 1024);

    // file size is 1024 (header) + image data size
    // image data size is 32 bits times width*height
    fi.total_file_size = 1024 + sizeof( boost::uint32_t) * view.width() * view.height();
    fi.total_file_size = IECore::asBigEndian( fi.total_file_size);

    out.write( reinterpret_cast<char *>( &fi), sizeof(fi));
    if ( out.fail())
		throw exception( "can't write header");

    out.write(reinterpret_cast<char *>(&ii),   sizeof(ii));
    if ( out.fail())
		throw exception( "can't write header");

    out.write(reinterpret_cast<char *>(&idfi), sizeof(idfi));
    if ( out.fail())
		throw exception( "can't write header");

    out.write(reinterpret_cast<char *>(&ioi),  sizeof(ioi));
    if ( out.fail())
		throw exception( "can't write header");

    write_pixels( out, view);
}

void cineon_writer_t::write_pixels( std::ofstream& out, const image::const_image_view_t& view) const
{
    std::vector<boost::uint32_t> buffer( view.width());

    for( int y = 0; y < view.height(); ++y)
    {
		image::const_image_view_t::x_iterator src_it( view.row_begin( y));
	
		for( int x = 0; x < view.width(); ++x)
		{
			boost::uint32_t pix = 0;
	
			float r = boost::gil::get_color( *src_it, boost::gil::red_t());
			float g = boost::gil::get_color( *src_it, boost::gil::green_t());
			float b = boost::gil::get_color( *src_it, boost::gil::blue_t());
	
			boost::uint32_t ri = adobe::clamp( r, 0.0f, 1.0f) * 1023;
			boost::uint32_t gi = adobe::clamp( g, 0.0f, 1.0f) * 1023;
			boost::uint32_t bi = adobe::clamp( b, 0.0f, 1.0f) * 1023;
			pix = ( ri << 22) | ( gi << 12) | ( bi << 2);
	
			buffer[x] = IECore::asBigEndian( pix);
			++src_it;
		}
	
		// write vector
		out.write( reinterpret_cast<char *>( &buffer[0]), view.width() * sizeof( boost::uint32_t));
		
		if ( out.fail())
			throw exception( "error while writting image");
    }
}

} // namespace
} // namespace
