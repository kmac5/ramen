// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/system/system_pvt.hpp>

#include<stdio.h>
#include<string.h>
#include<pwd.h>
#include<unistd.h>

#include<sys/sysinfo.h>
#include<sys/ioctl.h>
#include<sys/types.h>
#include<sys/socket.h>

#include<netinet/in.h>
#include<linux/if.h>

#include<boost/filesystem/operations.hpp>

#include<ramen/assert.hpp>

namespace ramen
{
namespace system
{
namespace
{
// global vars, but they are private.
std::string system_name_;
std::string user_name_;
boost::filesystem::path home_path_;
boost::filesystem::path app_bundle_path_;
boost::filesystem::path executable_path_;
boost::filesystem::path resource_path_;
boost::filesystem::path preferences_path_;
boost::filesystem::path tmp_path_;
boost::filesystem::path persistent_tmp_path_;
boost::filesystem::path app_user_path_;
cpu_family_type cpu_type_;
int simd_type_;
boost::uint64_t ram_size_;
boost::array<boost::uint8_t,6> mac_address_;

} // unnamed

system_pvt::system_pvt()
{
    struct passwd *p = getpwuid( geteuid());
    user_name_ = p->pw_name;
    home_path_ = p->pw_dir;

    // cpu info
    #if ( defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__)))
		#define cpuid(func,ax,bx,cx,dx)\
			__asm__ __volatile__ ("cpuid": "=a" (ax), "=b" (bx), "=c" (cx), "=d" (dx) : "a" (func));
	
		cpu_type_ = cpu_x86;
		simd_type_ = simd_none;
	
		int eax, ebx, ecx, edx;
		cpuid( 1, eax, ebx, ecx, edx);
		
		if( edx & 1 << 25)  simd_type_ |= simd_sse;
		if( edx & 1 << 26)  simd_type_ |= simd_sse2;
		if( ecx & 1)	    simd_type_ |= simd_sse3;
    #else
		cpu_type_ = cpu_unknown;
		simd_type_ = simd_none;
    #endif

    // get ram size
	for( int i = 0; i < 6; ++i)
		mac_address_[i] = 0;

    struct sysinfo info;
    sysinfo( &info);
    ram_size_ = info.totalram * info.mem_unit;
	
	// mac address
	int fd = socket( AF_INET, SOCK_DGRAM, 0);
	struct ifreq ifr;
	ifr.ifr_addr.sa_family = AF_INET;
	strncpy( ifr.ifr_name, "eth0", IFNAMSIZ-1);
	int err = ioctl( fd, SIOCGIFHWADDR, &ifr);
	close( fd);

	if( err != -1)
	{
		for( int i = 0; i < 6; ++i)
			mac_address_[i] = ifr.ifr_hwaddr.sa_data[i];
	}
}
	
const std::string& system_pvt::system_name() const
{
	if( system_name_.empty())
		system_name_ = "Linux";
	
	return system_name_;
}

const std::string& system_pvt::user_name() const { return user_name_;}

// paths
const boost::filesystem::path& system_pvt::home_path() const { return home_path_;}

const boost::filesystem::path& system_pvt::app_user_path() const
{
	if( app_user_path_.empty())
	    app_user_path_ =  home_path() / "ramen1.0";
	
	return app_user_path_;
}

const boost::filesystem::path& system_pvt::executable_path() const
{
    if( executable_path_.empty())
    {
		char linkname[128];

		pid_t pid = getpid();

		if( snprintf( linkname, sizeof( linkname), "/proc/%i/exe", pid) < 0)
			RAMEN_ASSERT( false);

		char buf[256];

		int ret = readlink( linkname, buf, sizeof( buf));

		if( ret == -1)
			RAMEN_ASSERT( false);

		if( ret >= sizeof( buf))
			RAMEN_ASSERT( false);

		buf[ret] = 0;
		executable_path_ = boost::filesystem::path( buf);
    }

    return executable_path_;
}

const boost::filesystem::path& system_pvt::app_bundle_path() const
{
	if( app_bundle_path_.empty())
	{
	    boost::filesystem::path p( executable_path());
	    app_bundle_path_ =  p.parent_path() / "../";
	}
	
	return app_bundle_path_;
}

const boost::filesystem::path& system_pvt::preferences_path() const
{
    if( preferences_path_.empty())
	{
		preferences_path_ = app_user_path() / "prefs";

	    if( !boost::filesystem::exists( preferences_path_))
            boost::filesystem::create_directories( preferences_path_);
	}

    return preferences_path_;
}

const boost::filesystem::path& system_pvt::tmp_path() const
{
    if( tmp_path_.empty())
		tmp_path_ = "/tmp/ramen";
	
	return tmp_path_;
}

const boost::filesystem::path& system_pvt::persistent_tmp_path() const
{
    if( persistent_tmp_path_.empty())
		persistent_tmp_path_ = "/var/tmp/ramen";

	return persistent_tmp_path_;
}

cpu_family_type system_pvt::cpu_type() const { return cpu_type_;}

int system_pvt::simd_type() const { return simd_type_;}

boost::uint64_t system_pvt::ram_size() const { return ram_size_;}

const boost::array<boost::uint8_t,6>& system_pvt::mac_address() const { return mac_address_;}

bool system_pvt::create_file_lock( const boost::filesystem::path& p) const
{
	return false;
}

void system_pvt::release_file_lock( const boost::filesystem::path& p) const
{
}

} // namespace
} // namespace
