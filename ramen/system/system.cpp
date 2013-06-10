// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/system/system.hpp>

#include<stdexcept>

#include<boost/filesystem/convenience.hpp>

#include<ramen/config/os.hpp>

#include<ramen/version.hpp>
#include<ramen/assert.hpp>

#if defined( RAMEN_CONFIG_OS_LINUX)
    #include<ramen/system/system_impl_linux.hpp>
#elif defined( RAMEN_CONFIG_OS_OSX)
    #include<ramen/system/system_impl_osx.hpp>
#elif defined( RAMEN_CONFIG_OS_WINDOWS)
    #include<ramen/system/system_impl_win.hpp>
#else
    #error "OS not supported"
#endif

namespace ramen
{
namespace system
{

system_t::system_t() : ram_size_( 0)
{
    pimpl_ = new impl( *this);

    // validate data
    if( system_name_.empty())
        throw std::runtime_error( "Error creating system class");

    if( user_name_.empty())
        throw std::runtime_error( "Error creating system class");

    if( !ram_size_)
        throw std::runtime_error( "Error creating system class");

    if( executable_path_.empty())
        throw std::runtime_error( "Error creating system class");

    if( home_path_.empty())
        throw std::runtime_error( "Error creating system class");

    if( application_user_path_.empty())
        throw std::runtime_error( "Error creating system class");

    application_path_ = boost::filesystem::canonical( executable_path_.parent_path() / "..");
}

system_t::~system_t()
{
    delete pimpl_;
}

} // system
} // ramen
