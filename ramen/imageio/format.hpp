// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGEIO_FORMAT_HPP
#define	RAMEN_IMAGEIO_FORMAT_HPP

#include<ramen/config.hpp>

#include<string>
#include<vector>

#include<ramen/core/memory.hpp>

#include<ramen/imageio/reader.hpp>
#include<ramen/imageio/writer.hpp>

namespace ramen
{
namespace imageio
{

struct RAMEN_API format_t
{
    format_t();
    virtual ~format_t();

    virtual bool input() const  { return true;}
    virtual bool output() const { return true;}

    virtual std::string tag() const = 0;

    virtual bool check_extension( const std::string& str) const;
    virtual void add_extensions( std::vector<std::string>& ext) const = 0;

    virtual std::size_t detect_size() const;
    virtual bool detect( const char *p) const;

    virtual bool is_multichannel() const;

    virtual core::auto_ptr_t<reader_t> reader( const boost::filesystem::path& p) const;
    virtual core::auto_ptr_t<writer_t> writer() const;

private:

    // non-copyable
    format_t( const format_t&);
    format_t& operator=( const format_t&);
};

} // imageio
} // ramen

#endif
