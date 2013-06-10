// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/imageio/flo/flo_format.hpp>
#include<ramen/imageio/flo/flo_reader.hpp>
#include<ramen/imageio/flo/flo_writer.hpp>

#include<ramen/imageio/factory.hpp>

namespace ramen
{
namespace imageio
{

flo_format_t::flo_format_t() {}

std::string flo_format_t::tag() const { return std::string( "flo");}

bool flo_format_t::check_extension( const std::string& str) const
{ 
    return str == ".flo" || str == ".flo";
}

std::size_t flo_format_t::detect_size() const { return 4;}

bool flo_format_t::detect( const char *p) const
{
	const float flo_tag = 202021.25f;
	const float *pp = reinterpret_cast<const float*>( p);
	return *pp == flo_tag;
}

void flo_format_t::add_extensions( std::vector<std::string>& ext) const
{
    ext.push_back( "flo");
    ext.push_back( "Flo");
}

std::auto_ptr<reader_t> flo_format_t::reader( const boost::filesystem::path& p) const
{
    std::auto_ptr<reader_t> r( new flo_reader_t( p));
    return r;
}

std::auto_ptr<writer_t> flo_format_t::writer() const
{
    std::auto_ptr<writer_t> w( new flo_writer_t());
    return w;
}

static bool registered = factory_t::instance().register_image_format( std::auto_ptr<format_t>( new flo_format_t()));

} // namespace
} // namespace
