// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/imageio/flo/flo_reader.hpp>

#include<OpenEXR/ImathVec.h>

#include<ramen/imageio/memory.hpp>

namespace ramen
{
namespace imageio
{

flo_reader_t::flo_reader_t( const boost::filesystem::path& p) : reader_t( p)
{
    adobe::auto_ptr<FILE> stream( fopen( filesystem::file_cstring( p), "rb"));

    if( !stream)
		throw unknown_image_format();
	
	const float flo_tag = 202021.25f;
    float tag;
    int width, height;

    if( fread( &tag, sizeof(float), 1, stream.get()) != 1 || 
		fread( &width,  sizeof(int), 1, stream.get()) != 1 || 
		fread( &height, sizeof(int), 1, stream.get()) != 1)
	{
		throw unknown_image_format();
	}
	
    if( tag != flo_tag)
		throw unknown_image_format();

    if( width < 1 || width > 99999 || height < 1 || height > 99999)
		throw unknown_image_format();
	
    info_[ adobe::name_t( "format")] = adobe::any_regular_t( Imath::Box2i( Imath::V2i( 0, 0), Imath::V2i( width-1, height-1)));
}

void flo_reader_t::do_read_image( const image::image_view_t& view, const Imath::Box2i& crop, int subsample) const
{
    adobe::auto_ptr<FILE> fp( fopen( filesystem::file_cstring( path_), "rb"));

    if ( !fp)
    {
		throw exception( "Can't open file");
		return;
    }
	
	const float flo_tag = 202021.25f;
    float tag;
    int width, height;

    if( fread( &tag, sizeof(float), 1, fp.get()) != 1 || 
		fread( &width,  sizeof(int), 1, fp.get()) != 1 || 
		fread( &height, sizeof(int), 1, fp.get()) != 1)
	{
		throw unknown_image_format();
	}
	
    if( tag != flo_tag)
		throw unknown_image_format();

    if( width < 1 || width > 99999 || height < 1 || height > 99999)
		throw unknown_image_format();
	
    std::vector<Imath::V2f> scanline_buffer( width);
	Imath::V2f *ptr = &( scanline_buffer[0]);

	// skip first scanlines
	fseek( fp.get(), width * sizeof( float) * crop.min.y, SEEK_CUR);
	
	int y = crop.min.y;
    int yy = 0;

    while( y <= crop.max.y)
    {
		if( fread( ptr, sizeof( Imath::V2f), width, fp.get()) != width)
			throw exception( "Can't read file");			
		
		// process one scanline
		image::image_view_t::x_iterator dst_it( view.row_begin( yy));
		image::image_view_t::x_iterator dst_end( view.row_end( yy));

		Imath::V2f *q = ptr + crop.min.x;
		Imath::V2f *q_end = ptr + crop.max.x + 1;

		for( ; dst_it != dst_end; ++dst_it)
		{
			boost::gil::get_color( *dst_it, boost::gil::red_t())	= q[0].x * subsample;
			boost::gil::get_color( *dst_it, boost::gil::green_t())	= q[0].y * subsample;
			boost::gil::get_color( *dst_it, boost::gil::blue_t())	= 0.0f;
			boost::gil::get_color( *dst_it, boost::gil::alpha_t())	= 0.0f;
			q += subsample;
		
			if( q >= q_end)
				q = ptr + crop.max.x;
		}

		++yy;

		// skip subsampled scanlines
		if( subsample != 1)
		{
			if( y + subsample - 1 <= crop.max.y)
				fseek( fp.get(), width * sizeof( float) * ( subsample - 1), SEEK_CUR);

			y += subsample - 1;
		}
		
		++y;
	}

	repeat_scanline_until_end( view, yy - 1);
}

} // namespace
} // namespace
