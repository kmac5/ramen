// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_SYSTEM_HPP
#define RAMEN_SYSTEM_HPP

#include<memory>

#include<ramen/app/application_fwd.hpp>

#include<ramen/system/system_pvt.hpp>

namespace ramen
{
namespace system
{

/**
\ingroup app
\brief class that contains os & hardware related data and methods
*/
class system_t : boost::noncopyable
{
public:

    const std::string& system_name() const;
    const std::string& user_name() const;
	
    // paths
    const boost::filesystem::path& home_path() const;
    const boost::filesystem::path& app_user_path() const;
    const boost::filesystem::path& executable_path() const;
    const boost::filesystem::path& app_bundle_path() const;
    const boost::filesystem::path& preferences_path() const;
    const boost::filesystem::path& tmp_path() const;
    const boost::filesystem::path& persistent_tmp_path() const;

    // cpu
    cpu_family_type cpu_type() const;
    int simd_type() const;

    // ram
    boost::uint64_t ram_size() const;

	// mac address
	const boost::array<boost::uint8_t,6>& mac_address() const;
	std::string mac_address_as_string() const;
	
	// file locks
	bool create_file_lock( const boost::filesystem::path& p) const;
	void release_file_lock( const boost::filesystem::path& p) const;

private:

    friend class ramen::application_t;

    system_t();

	std::auto_ptr<system_pvt> pimpl_;
};

} // namespace
} // namespace

#endif
