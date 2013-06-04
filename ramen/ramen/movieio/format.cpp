// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/movieio/format.hpp>

namespace ramen
{
namespace movieio
{

format_t::format_t() {}
format_t::~format_t() {}

bool format_t::check_extension( const std::string& str) const { return false;}

std::size_t format_t::detect_size() const     { return 0;}
bool format_t::detect( const char *p) const   { return false;}

std::auto_ptr<reader_t> format_t::reader( const boost::filesystem::path& p) const	{ return std::auto_ptr<reader_t>();}
std::auto_ptr<writer_t> format_t::writer() const									{ return std::auto_ptr<writer_t>();}

} // namespace
} // namespace
