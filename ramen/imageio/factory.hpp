// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGEIO_FACTORY_HPP
#define	RAMEN_IMAGEIO_FACTORY_HPP

#include<ramen/config.hpp>

#include<vector>
#include<string>

#include<ramen/core/memory.hpp>

#include<boost/ptr_container/ptr_vector.hpp>

#include<ramen/imageio/exceptions.hpp>
#include<ramen/imageio/enums.hpp>
#include<ramen/imageio/format.hpp>

#include<ramen/image/typedefs.hpp>

namespace ramen
{
namespace imageio
{

class RAMEN_API factory_t
{
public:

    static factory_t& instance();

    ~factory_t();

    const boost::ptr_vector<format_t>& formats() const { return formats_;}

    typedef boost::ptr_vector<format_t>::const_iterator const_iterator;

    const_iterator begin() const    { return formats_.begin();}
    const_iterator end() const	    { return formats_.end();}

    bool register_image_format( core::auto_ptr_t<format_t> format);

    const std::vector<std::string>& extensions() const  { return extensions_;}

	bool is_image_file( const boost::filesystem::path& p) const;
	bool is_image_format_tag( const std::string& tag) const;
	
    core::auto_ptr_t<reader_t> reader_for_image( const boost::filesystem::path& p) const;
    core::auto_ptr_t<writer_t> writer_for_tag( const std::string& tag) const;

private:

    factory_t();

    // non-copyable
    factory_t( const factory_t&);
    factory_t& operator=( const factory_t&);

    const_iterator format_for_tag( const std::string& tag) const;
    const_iterator format_for_extension( const boost::filesystem::path& p) const;
    const_iterator format_for_file_contents( const boost::filesystem::path& p) const;

    boost::ptr_vector<format_t> formats_;
    std::vector<std::string> extensions_;

    std::size_t detect_size_;
    mutable char *detect_buffer_;
};

} // imageio
} // ramen

#endif
