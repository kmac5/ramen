// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/imageio/factory.hpp>

#include<algorithm>

#include<boost/foreach.hpp>
#include<boost/filesystem/fstream.hpp>

#include<ramen/imageio/algorithm.hpp>
#include<ramen/imageio/exceptions.hpp>

namespace ramen
{
namespace imageio
{

factory_t& factory_t::instance()
{
    static factory_t f;
    return f;
}

factory_t::factory_t() : detect_size_(0), detect_buffer_(0) {}
factory_t::~factory_t() { delete detect_buffer_;}

bool factory_t::register_image_format( core::auto_ptr_t<format_t> format)
{
    detect_size_ = std::max( detect_size_, format->detect_size());
    format->add_extensions( extensions_);
    formats_.push_back( format.release());
    return true;
}

bool factory_t::is_image_file( const boost::filesystem::path& p) const
{
    const_iterator it = format_for_extension( p);

    if( it != formats_.end())
		return true;

    it = format_for_file_contents( p);

    if( it != formats_.end())
		return true;

	return false;
}

bool factory_t::is_image_format_tag( const std::string& tag) const
{
	return format_for_tag( tag) != formats_.end();
}

core::auto_ptr_t<reader_t> factory_t::reader_for_image( const boost::filesystem::path& p) const
{
    const_iterator it = format_for_extension( p);

    if( it != formats_.end())
		return it->reader( p);

    it = format_for_file_contents( p);

    if( it != formats_.end())
		return it->reader( p);

	throw unknown_image_format();
    return core::auto_ptr_t<reader_t>();
}

core::auto_ptr_t<writer_t> factory_t::writer_for_tag( const std::string& tag) const
{
    const_iterator it = format_for_tag( tag);

    if( it != formats_.end())
        return core::auto_ptr_t<writer_t>( it->writer());

	throw unknown_image_format();
    return core::auto_ptr_t<writer_t>();
}

factory_t::const_iterator factory_t::format_for_extension( const boost::filesystem::path& p) const
{
    std::string ext( p.extension().string());

    if( !ext.empty())
    {
        for( const_iterator it( formats_.begin()); it != formats_.end(); ++it)
        {
            if( it->input() &&  it->check_extension( ext))
            return it;
        }
    }

    return formats_.end();
}

factory_t::const_iterator factory_t::format_for_file_contents( const boost::filesystem::path& p) const
{
    boost::filesystem::ifstream ifile( p, std::ios::in | std::ios::binary);

    if( ifile.is_open() && ifile.good())
	{
	    if( !detect_buffer_)
	        detect_buffer_ = new char[detect_size_];

	    ifile.read( detect_buffer_, detect_size_);
	
	    for( const_iterator it( formats_.begin()); it != formats_.end(); ++it)
	    {
	        if( it->input() && it->detect( detect_buffer_))
	            return it;
	    }
	}

    return formats_.end();
}

factory_t::const_iterator factory_t::format_for_tag( const std::string& tag) const
{
    for( const_iterator it( formats_.begin()); it != formats_.end(); ++it)
    {
        if( it->output() && ( it->tag() == tag))
            return it;
    }

    return formats_.end();
}

} // imageio
} // ramen
