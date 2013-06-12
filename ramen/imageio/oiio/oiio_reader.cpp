// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/imageio/oiio/oiio_reader.hpp>

#include<boost/scoped_array.hpp>
#include<boost/range.hpp>

#include<ramen/imageio/algorithm.hpp>

namespace ramen
{
namespace imageio
{

oiio_reader_t::oiio_reader_t( const boost::filesystem::path& p) : reader_t( p), is_tiled_( false)
{
    std::auto_ptr<OIIO::ImageInput> in( OIIO::ImageInput::create( filesystem::file_string( p)));

    if( in.get())
    {
		OIIO::ImageSpec spec;
				
		if( in->open( filesystem::file_string( p), spec))
		{
            info_[core::name_t( "format")] = core::variant_t( math::box2i_t( math::point2i_t( spec.full_x, spec.full_y),
                                                                             math::point2i_t( spec.full_x + spec.full_width - 1,
                                                                                               spec.full_y + spec.full_height - 1)));

            info_[core::name_t( "bounds")] = core::variant_t( math::box2i_t( math::point2i_t( spec.x, spec.y),
                                                                             math::point2i_t( spec.x + spec.width - 1,
																							   spec.y + spec.height - 1)));
			
            info_[core::name_t( "aspect")] = core::variant_t( spec.get_float_attribute( "pixelAspectRatio", 1.0f));
			
			if( spec.tile_width != 0)
				is_tiled_  = true;
		}
		else
			throw unknown_image_format();
	
		in->close();
    }
    else
		throw unknown_image_format();
}

void oiio_reader_t::do_read_image( const image::image_view_t& view,
                                   const math::box2i_t& crop,
                                   int subsample) const
{
	if( is_tiled_)
	{
		do_read_tiled_image( view, crop, subsample);
		return;
	}
	
    std::auto_ptr<OIIO::ImageInput> in( OIIO::ImageInput::create( filesystem::file_string( path_)));

    if( in.get())
    {
		OIIO::ImageSpec spec, config;
		config.attribute( "oiio:UnassociatedAlpha", ( int) 1);

		if( in->open( filesystem::file_string( path_), spec, config))
		{
		    boost::scoped_array<float> buffer( new float[ spec.width * spec.nchannels]);

            int yy = 0;
		    for( int y = crop.min.y; y <= crop.max.y; y += subsample)
		    {
				in->read_scanline( y, 0, OIIO::TypeDesc::FLOAT, buffer.get());

				image::image_view_t::x_iterator dst_it( view.row_begin( yy));
				float *q     = buffer.get() + ( crop.min.x * spec.nchannels);
				float *q_end = buffer.get() + (( crop.max.x + 1) * spec.nchannels);
		
				switch( spec.nchannels)
				{
                    case 1:
                        for( ; dst_it != view.row_end( yy); ++dst_it)
                        {
                            boost::gil::get_color( *dst_it, boost::gil::red_t())	= q[0];
                            boost::gil::get_color( *dst_it, boost::gil::green_t())	= q[0];
                            boost::gil::get_color( *dst_it, boost::gil::blue_t())	= q[0];
                            boost::gil::get_color( *dst_it, boost::gil::alpha_t())	= 1.0f;
                            q += ( spec.nchannels * subsample);

                            if( q >= q_end)
                                q = buffer.get() + ( crop.max.x * spec.nchannels);
                        }
                    break;

                    case 3:
                        for( ; dst_it != view.row_end( yy); ++dst_it)
                        {
                            boost::gil::get_color( *dst_it, boost::gil::red_t())	= q[0];
                            boost::gil::get_color( *dst_it, boost::gil::green_t())	= q[1];
                            boost::gil::get_color( *dst_it, boost::gil::blue_t())	= q[2];
                            boost::gil::get_color( *dst_it, boost::gil::alpha_t())	= 1.0f;
                            q += ( spec.nchannels * subsample);

                            if( q >= q_end)
                                q = buffer.get() + ( crop.max.x * spec.nchannels);
                        }
                    break;

                    case 4:
                        for( ; dst_it != view.row_end( yy); ++dst_it)
                        {
                            boost::gil::get_color( *dst_it, boost::gil::red_t())	= q[0];
                            boost::gil::get_color( *dst_it, boost::gil::green_t())	= q[1];
                            boost::gil::get_color( *dst_it, boost::gil::blue_t())	= q[2];
                            boost::gil::get_color( *dst_it, boost::gil::alpha_t())	= q[3];
                            q += ( spec.nchannels * subsample);

                            if( q >= q_end)
                                q = buffer.get() + ( crop.max.x * spec.nchannels);
                        }
                    break;
				}

				++yy;
		    }

			repeat_scanline_until_end( view, yy - 1);
		}
		else
		    throw exception( "Can't open file");

		in->close();
    }
}

void oiio_reader_t::do_read_tiled_image( const image::image_view_t& view,
                                         const math::box2i_t& crop,
                                         int subsample) const
{
    std::auto_ptr<OIIO::ImageInput> in( OIIO::ImageInput::create( filesystem::file_string( path_)));

    if( in.get())
    {
		OIIO::ImageSpec spec, config;
		config.attribute( "oiio:UnassociatedAlpha", ( int) 1);

		if( in->open( filesystem::file_string( path_), spec, config))
		{
			if( ( spec.tile_width & 1) || ( spec.tile_height & 1))
				throw std::runtime_error( "Odd tile sizes not supported");
			
		    boost::scoped_array<float> buffer( new float[ spec.tile_height * spec.tile_width * spec.nchannels]);

			for( int y = spec.y; y < spec.y + spec.height; y += spec.tile_height)
			{
				for( int x = spec.x; x < spec.x + spec.width; x += spec.tile_width)
				{
					// test if the tile is inside the crop area
					if( x <= crop.max.x && y <= crop.max.y && x + spec.tile_width >= crop.min.x && y + spec.tile_height >= crop.min.y)
					{
						in->read_tile( x, y, 0, OIIO::TypeDesc::FLOAT, buffer.get());
						copy_tile( x, y, spec, buffer.get(), crop, view, subsample);						
					}
				}
			}
		}
		else
			throw exception( "Can't open file");
		
		in->close();
	}
}

void oiio_reader_t::copy_tile( int x,
                               int y,
                               const OIIO::ImageSpec& spec,
                               float *data,
                               const math::box2i_t& crop,
                               const image::image_view_t& view,
                               int subsample) const
{
	std::size_t xstride = spec.nchannels;
	std::size_t ystride = spec.tile_width * xstride;
	
	int tile_startx = std::max( crop.min.x - x, 0);
	int tile_starty = std::max( crop.min.y - y, 0);
	
	int startx = std::max(x - crop.min.x, 0) / subsample;
	int starty = std::max(y - crop.min.y, 0) / subsample;
	int endx = std::min(( x + spec.tile_width  - crop.min.x) / subsample, ( int) view.width());
	int endy = std::min(( y + spec.tile_height - crop.min.y) / subsample, ( int) view.height());
	int dst_width  = endx - startx;
	int dst_height = endy - starty;

	for( int sy = 0; sy < dst_height; ++sy)
	{
		for( int sx = 0; sx < dst_width; ++sx)
		{
			// map to tile coordinates
			int ty = ( sy * subsample);
			int tx = ( sx * subsample);
			float *p = data + ( tile_starty + ty) * ystride + ( tile_startx + tx) * xstride;
			
			switch( spec.nchannels)
			{
				case 1:
					view( startx + sx, starty + sy) = image::pixel_t( *p, *p, *p, 1.0f);
				break;

				case 3:
					view( startx + sx, starty + sy) = image::pixel_t( p[0], p[1], p[2], 1.0f);
				break;

				case 4:
					view( startx + sx, starty + sy) = image::pixel_t( p[0], p[1], p[2], p[3]);
				break;
			}
		}
	}
}

} // imageio
} // ramen
