// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/imageio/exr/exr_reader.hpp>

#include<OpenEXR/ImfTestFile.h>
#include<OpenEXR/ImfInputFile.h>
#include<OpenEXR/ImfChannelList.h>
#include<OpenEXR/ImfRgbaFile.h>
#include<OpenEXR/ImfArray.h>

namespace ramen
{
namespace imageio
{
namespace
{
	
void copy_scanline( const image::image_view_t& view,
                    unsigned int src_line,
                    unsigned int dst_line)
{
	RAMEN_ASSERT( src_line >= 0);
	RAMEN_ASSERT( src_line < view.height());
	RAMEN_ASSERT( dst_line >= 0);
	RAMEN_ASSERT( dst_line < view.height());
	RAMEN_ASSERT( src_line != dst_line);
	
	std::copy( view.row_begin( src_line), view.row_end( src_line), view.row_begin( dst_line));
}
	
void copy_subsampled_scanline( image::image_view_t::x_iterator src_begin,
                               image::image_view_t::x_iterator src_end,
                               image::image_view_t::x_iterator dst_begin,
                               image::image_view_t::x_iterator dst_end,
                               int subsample)
{
	image::image_view_t::x_iterator src_it = src_begin;
	image::image_view_t::x_iterator dst_it = dst_begin;

	while( src_it < src_end)
	{
		if( dst_it == dst_end)
			return;

		*dst_it++ = *src_it;
		src_it += subsample;
	}

	// repeat the last pixel until we fill the scan line
	while( dst_it < dst_end)
		*dst_it++ = *(src_end - 1);
}
	
void copy_and_convert_subsampled_scanline( image::image_view_t::x_iterator src_begin,
                                           image::image_view_t::x_iterator src_end,
                                           image::image_view_t::x_iterator dst_begin,
                                           image::image_view_t::x_iterator dst_end,
                                           int subsample)
{
	image::image_view_t::x_iterator src_it = src_begin;
	image::image_view_t::x_iterator dst_it = dst_begin;

	while( src_it < src_end)
	{
		boost::gil::color_convert( *src_it, *dst_it++);
		src_it += subsample;
	}

	// repeat the last pixel until we fill the scan line
	while( dst_it < dst_end)
		boost::gil::color_convert( *(src_end - 1), *dst_it++);
}

math::box2i_t convert_box( const Imath::Box2i& box)
{
    return math::box2i_t( math::point2i_t( box.min.x, box.min.y),
                          math::point2i_t( box.max.x, box.max.y));
}

} // unnamed

exr_reader_t::exr_reader_t( const boost::filesystem::path& p) : multichannel_reader_t( p)
{
    bool tiled;

    if(!Imf::isOpenExrFile( filesystem::file_cstring( p), tiled))
		throw exception( "Can't open file");

	Imf::InputFile ifile( filesystem::file_cstring( p));
	header_ = ifile.header();

    Imath::Box2i display_window( header_.displayWindow());
    info_[core::name_t( "format")] = core::variant_t( convert_box( display_window));

    Imath::Box2i data_window( header_.dataWindow());
    info_[core::name_t( "bounds")] = core::variant_t( convert_box( data_window));

    info_[core::name_t( "aspect")] = core::variant_t( header_.pixelAspectRatio());
	
    is_rgb_image_ = is_rgb();
    if( is_rgb_image_)
    {
		add_channel_name( "R");
		add_channel_name( "G");
		add_channel_name( "B");
		add_channel_name( "A");
		is_luma_chroma_image_ = false;
    }
    else
    {
		is_luma_chroma_image_ = is_luma_chroma();

		if( is_luma_chroma_image_)
		{
			add_channel_name( "R");
			add_channel_name( "G");
			add_channel_name( "B");
			add_channel_name( "A");
		}
    }

    // add channel names
    const Imf::ChannelList& ch_list( header_.channels());

    for( Imf::ChannelList::ConstIterator it( ch_list.begin()); it != ch_list.end(); ++it)
    {
		std::string name( it.name());

		if( is_rgb_image_)
		{
			if( name == "R")
				continue;

			if( name == "G")
				continue;

			if( name == "B")
				continue;

			if( name == "A")
				continue;
		}
		else
		{
			if( is_luma_chroma_image_)
			{
				if( name == "Y")
					continue;

				if( name == "RY")
					continue;

				if( name == "BY")
					continue;

				if( name == "A")
					continue;
			}
		}

		add_channel_name( it.name());
    }

    if( is_rgb_image_ || is_luma_chroma_image_)
		has_extra_channels_ = ( channel_list().size() != 4);
    else
		has_extra_channels_ = true;
}

bool exr_reader_t::is_rgb() const
{
    const Imf::ChannelList& ch_list( header_.channels());
    return ch_list.findChannel( "R") && ch_list.findChannel( "G") && ch_list.findChannel( "B");
}

bool exr_reader_t::is_luma_chroma() const
{
    const Imf::ChannelList& ch_list( header_.channels());
    return ch_list.findChannel( "Y");
}

void exr_reader_t::do_read_image( const image::image_view_t& view,
                                  const math::box2i_t& crop,
                                  int subsample,
                                  const boost::tuple<int,int,int,int>& channels) const
{
    // this is to handle luma-chroma image
    if( !has_extra_channels())
    {
		read_exr_image( path_, view, crop, subsample);
		return;
    }

    const char *rchannel = 0;
    int indx = boost::get<0>( channels);

    if( indx < channel_list().size())
		rchannel = channel_list()[indx].c_str();

    const char *gchannel = 0;
    indx = boost::get<1>( channels);

    if( indx < channel_list().size())
		gchannel = channel_list()[indx].c_str();

    const char *bchannel = 0;
    indx = boost::get<2>( channels);

    if( indx < channel_list().size())
		bchannel = channel_list()[indx].c_str();

    const char *achannel = 0;
    indx = boost::get<3>( channels);

    if( indx < channel_list().size())
		achannel = channel_list()[indx].c_str();

	read_exr_image( path_, view, crop, rchannel, gchannel, bchannel, achannel, subsample);
}

void exr_reader_t::read_exr_image( const boost::filesystem::path& p,
                                   const image::image_view_t& result_view,
                                   const math::box2i_t& crop,
                                   std::size_t subsample) const
{
	if( is_luma_chroma_image_)
		read_exr_luma_chroma_image( path_, result_view, crop, subsample);
	else
	    read_exr_image( p, result_view, crop, "R", "G", "B", "A", subsample);
}

void exr_reader_t::read_exr_image( const boost::filesystem::path& p,
                                   const image::image_view_t& result_view,
                                   const math::box2i_t& crop,
                                   const char *rchannel,
                                   const char *gchannel,
                                   const char *bchannel,
                                   const char *achannel,
                                   std::size_t subsample) const
{
    Imf::InputFile file( filesystem::file_cstring( p));
    Imath::Box2i data_window( file.header().dataWindow());

    std::size_t width  = data_window.max.x - data_window.min.x + 1;
    std::size_t xstride = 4 * sizeof(float);
    std::size_t ystride = width * xstride;

    boost::gil::rgba32f_image_t scan_line( width, 1);
    boost::gil::fill_pixels( boost::gil::view( scan_line), image::pixel_t( 0, 0, 0, 1));
    float *buffer = (float *) boost::gil::interleaved_view_get_raw_data( boost::gil::view( scan_line));

    RAMEN_ASSERT( result_view.width() >= std::ceil( (float) (crop.size().x+1) / subsample));
    RAMEN_ASSERT( result_view.height() >= std::ceil( (float) (crop.size().y+1) / subsample));

    Imf::FrameBuffer frameBuffer;

    int dst_y = 0;
    for( int y = crop.min.y; y <= crop.max.y; y += subsample)
    {
		char *ptr = ((char *) buffer) - (y * ystride) - (data_window.min.x * xstride);

		if( rchannel)
		    frameBuffer.insert( rchannel, Imf::Slice( Imf::FLOAT, ptr , xstride, ystride));

		ptr += sizeof( float);
		
		if( gchannel)
			frameBuffer.insert( gchannel, Imf::Slice( Imf::FLOAT, ptr , xstride, ystride));
		
		ptr += sizeof( float);
		
		if( bchannel)
			frameBuffer.insert( bchannel, Imf::Slice( Imf::FLOAT, ptr , xstride, ystride));
		
		ptr += sizeof( float);
		
		if( achannel)
			frameBuffer.insert( achannel, Imf::Slice( Imf::FLOAT, ptr , xstride, ystride, 1, 1, 1.0));
		
		file.setFrameBuffer( frameBuffer);
		file.readPixels( y);
		
		image::image_view_t::x_iterator dst_begin( result_view.row_begin( dst_y));
		image::image_view_t::x_iterator dst_end( result_view.row_end( dst_y));
		
		image::image_view_t::x_iterator src_begin( boost::gil::view( scan_line).row_begin( 0) + crop.min.x - data_window.min.x);
        image::image_view_t::x_iterator src_end( src_begin + crop.size().x + 1);

        copy_subsampled_scanline( src_begin, src_end, dst_begin, dst_end, subsample);
		++dst_y;
    }

    int last_scan = dst_y - 1;

    while( dst_y < result_view.height())
    {
        copy_scanline( result_view, last_scan, dst_y);
        ++dst_y;
    }
}

void exr_reader_t::read_exr_luma_chroma_image( const boost::filesystem::path& p,
                                               const image::image_view_t& result_view,
                                               const math::box2i_t& crop,
                                               std::size_t subsample) const
{
    // TODO: optimize this, read only scanlines needed
    RAMEN_ASSERT( result_view.width() >= std::ceil( (float) (crop.size().x+1) / subsample));
    RAMEN_ASSERT( result_view.height() >= std::ceil( (float) (crop.size().y+1) / subsample));

    Imf::RgbaInputFile file( filesystem::file_cstring( p));
    Imath::Box2i data_window( file.header().dataWindow());

    std::size_t width  = data_window.max.x - data_window.min.x + 1;
    std::size_t height = data_window.max.y - data_window.min.y + 1;

    boost::gil::rgba16f_image_t buffer( width, height);
    Imf::Rgba *ptr = (Imf::Rgba *) boost::gil::interleaved_view_get_raw_data( boost::gil::view( buffer));

    file.setFrameBuffer( ptr - data_window.min.x - (data_window.min.y * width), 1, width);
    
    file.readPixels( data_window.min.y, data_window.max.y);

    boost::gil::rgba16fc_view_t src_view( boost::gil::const_view( buffer));

    for( int j = 0; j < result_view.height(); ++j)
    {
		int src_y = crop.min.y - data_window.min.y + (j * subsample);

		if( src_y >= src_view.height())
		    return;
	
		boost::gil::rgba16fc_view_t::x_iterator src_it  = src_view.row_begin( src_y);
		src_it += crop.min.x - data_window.min.x;

		boost::gil::rgba16fc_view_t::x_iterator src_end  = src_view.row_begin( src_y);
		src_end += crop.max.x - data_window.min.x + 1;

		image::image_view_t::x_iterator dst_it( result_view.row_begin( j));

		for( int i = 0; i < result_view.width(); ++i)
		{
		    if( src_it >= src_end)
				break;

		    boost::gil::color_convert( *src_it, *dst_it++);
		    src_it += subsample;
		}
    }
}

} // imageio
} // ramen
