// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/imageio/tiff/tiff_format.hpp>

#include<ramen/imageio/oiio/oiio_reader.hpp>

#include<ramen/imageio/tiff/tiff_writer.hpp>

#include<ramen/imageio/factory.hpp>

namespace ramen
{
namespace imageio
{

tiff_format_t::tiff_format_t() {}
tiff_format_t::~tiff_format_t() {}

std::string tiff_format_t::tag() const { return std::string( "tiff");}

bool tiff_format_t::check_extension( const std::string& str) const
{ 
    return str == ".tif" || str == ".tiff" || str == ".TIF" || str == ".TIFF";
}

std::size_t tiff_format_t::detect_size() const { return 4;}

bool tiff_format_t::detect( const char *p) const
{
    const unsigned char *pp = reinterpret_cast<const unsigned char*>( p);
    if( ( pp[0] == 0x4D) && ( pp[1] == 0x4D) && ( pp[2] == 0x00) && ( pp[3] == 0x2A))
	return true;

    if( ( pp[0] == 0x49) && ( pp[1] == 0x49) && ( pp[2] == 0x2A) && ( pp[3] == 0x00))
	return true;

    return false;
}

void tiff_format_t::add_extensions( std::vector<std::string>& ext) const
{
    ext.push_back( "tif");
    ext.push_back( "tiff");
    ext.push_back( "TIF");
    ext.push_back( "TIFF");
}

core::auto_ptr_t<reader_t> tiff_format_t::reader( const boost::filesystem::path& p) const
{
    return core::auto_ptr_t<reader_t>( new oiio_reader_t( p));
}

core::auto_ptr_t<writer_t> tiff_format_t::writer() const
{
    return core::auto_ptr_t<writer_t>( new tiff_writer_t());
}

static bool registered = factory_t::instance().register_image_format( core::auto_ptr_t<format_t>( new tiff_format_t()));

} // imageio
} // ramen
