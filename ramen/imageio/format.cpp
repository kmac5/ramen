// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/imageio/format.hpp>

namespace ramen
{
namespace imageio
{

format_t::format_t() {}
format_t::~format_t() {}

bool format_t::check_extension( const std::string& str) const { return false;}

std::size_t format_t::detect_size() const     { return 4;}
bool format_t::detect( const char *p) const   { return false;}

bool format_t::is_multichannel() const { return false;}

core::auto_ptr_t<reader_t> format_t::reader( const boost::filesystem::path& p) const
{
    return core::auto_ptr_t<reader_t>();
}

core::auto_ptr_t<writer_t> format_t::writer() const
{
    return core::auto_ptr_t<writer_t>();
}

} // imageio
} // ramen
