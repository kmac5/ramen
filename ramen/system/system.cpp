// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/system/system.hpp>

#include<sstream>
#include<iomanip>

namespace ramen
{
namespace system
{

system_t::system_t() { pimpl_.reset( new system_pvt());}

const std::string& system_t::system_name() const	{ return pimpl_->system_name();}
const std::string& system_t::user_name() const	{ return pimpl_->user_name();}

// paths
const boost::filesystem::path& system_t::home_path() const { return pimpl_->home_path();}

const boost::filesystem::path& system_t::app_user_path() const { return pimpl_->app_user_path();}

const boost::filesystem::path& system_t::executable_path() const
{
    return pimpl_->executable_path();
}

const boost::filesystem::path& system_t::app_bundle_path() const
{
	return pimpl_->app_bundle_path();
}

const boost::filesystem::path& system_t::preferences_path() const
{
    return pimpl_->preferences_path();
}

const boost::filesystem::path& system_t::tmp_path() const
{
	return pimpl_->tmp_path();
}

const boost::filesystem::path& system_t::persistent_tmp_path() const
{
	return pimpl_->persistent_tmp_path();
}

// cpu
cpu_family_type system_t::cpu_type() const	{ return pimpl_->cpu_type();}

int system_t::simd_type() const { return pimpl_->simd_type();}

// ram
boost::uint64_t system_t::ram_size() const { return pimpl_->ram_size();}

// mac address
const boost::array<boost::uint8_t,6>& system_t::mac_address() const { return pimpl_->mac_address();}

std::string system_t::mac_address_as_string() const
{
    std::stringstream s;

    for( int i = 0; i < 6; ++i)
		s << std::setfill( '0') << std::setw( 2) << std::hex << (int) mac_address()[i];

    return s.str();	
}

bool system_t::create_file_lock( const boost::filesystem::path& p) const { return pimpl_->create_file_lock( p);}

void system_t::release_file_lock( const boost::filesystem::path& p)	 const { pimpl_->release_file_lock( p);}

} // namespace
} // namespace
