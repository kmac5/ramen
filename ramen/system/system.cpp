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

// cpu
cpu_family_type system_t::cpu_type() const	{ return pimpl_->cpu_type();}

int system_t::simd_type() const { return pimpl_->simd_type();}

// ram
boost::uint64_t system_t::ram_size() const { return pimpl_->ram_size();}

} // system
} // ramen
