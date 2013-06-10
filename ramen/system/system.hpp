// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_SYSTEM_HPP
#define RAMEN_SYSTEM_HPP

#include<ramen/config.hpp>

#include<string>

#include<ramen/app/application_fwd.hpp>

#include<boost/filesystem/path.hpp>

namespace ramen
{
namespace system
{

/**
\ingroup app
\brief class that contains os & hardware related data and methods
*/
class RAMEN_API system_t
{
public:

    const std::string& system_name() const
    {
        return system_name_;
    }

    const std::string& user_name() const
    {
        return user_name_;
    }

    const boost::filesystem::path& home_path() const
    {
        return home_path_;
    }

    const boost::filesystem::path& executable_path() const
    {
        return executable_path_;
    }

    const boost::filesystem::path& application_path() const
    {
        return application_path_;
    }

    const boost::filesystem::path& application_user_path() const
    {
        return application_user_path_;
    }

    // ram
    boost::uint64_t ram_size() const
    {
        return ram_size_;
    }

private:

    friend class ramen::application_t;

    system_t();
    ~system_t();

    // non-copyable
    system_t( const system_t&);
    system_t& operator=( const system_t&);

    std::string system_name_;
    std::string user_name_;

    boost::uint64_t ram_size_;

    // paths
    boost::filesystem::path home_path_;
    boost::filesystem::path executable_path_;
    boost::filesystem::path application_path_;
    boost::filesystem::path application_user_path_;

    struct impl;
    impl *pimpl_;
};

} // system
} // ramen

#endif
