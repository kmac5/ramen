// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/imageio/flo/flo_writer.hpp>

#include<OpenEXR/ImathVec.h>

#include<ramen/imageio/memory.hpp>

namespace ramen
{
namespace imageio
{

void flo_writer_t::do_write_image( const boost::filesystem::path& p,
				const image::const_image_view_t& view,
				const adobe::dictionary_t& params) const
{
    adobe::auto_ptr<FILE> fp( fopen( filesystem::file_cstring( p), "wb"));

    if( !fp)
		throw exception( "Can't open file");

	int width = view.width();
	int height = view.height();
	
    // write the header
    fprintf( fp.get(), "PIEH");
	
    if( fwrite( &width, sizeof( int), 1, fp.get()) != 1 || fwrite( &height, sizeof( int), 1, fp.get()) != 1)
		throw exception( "Can't write file");

    std::vector<Imath::V2f> buffer( width);
	
    for( int y = 0; y < view.height(); ++y)
    {
		Imath::V2f *ptr = &buffer.front();
		image::const_image_view_t::x_iterator src_it( view.row_begin( y));

		for( int x = 0; x < view.width(); ++x)
		{
		    ptr->x = boost::gil::get_color( *src_it, boost::gil::red_t());
		    ptr->y = boost::gil::get_color( *src_it, boost::gil::green_t());
			++ptr;
		    ++src_it;
		}

		ptr = &buffer.front();
		
		if( fwrite( ptr, sizeof( Imath::V2f), width, fp.get()) != width)
			throw exception( "Can't write file");
    }
}
    
} // namespace
} // namespace
