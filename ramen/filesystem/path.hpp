// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_FILESYSTEM_PATH_HPP
#define RAMEN_FILESYSTEM_PATH_HPP

#include<boost/filesystem/path.hpp>

namespace ramen
{
namespace filesystem
{

const std::string& file_string( const boost::filesystem::path& p);
const char *file_cstring( const boost::filesystem::path& p);

std::string hash_string( const boost::filesystem::path& p);

boost::filesystem::path make_absolute_path( const boost::filesystem::path& p,
                                            const boost::filesystem::path& from);

boost::filesystem::path make_relative_path( const boost::filesystem::path& p,
                                            const boost::filesystem::path& from);

boost::filesystem::path convert_relative_path( const boost::filesystem::path& p,
                                               const boost::filesystem::path& old_base,
                                                const boost::filesystem::path& new_base);

} // filesystem
} // ramen

#endif
