// Copyright (c) 2013 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

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

namespace ramen
{
namespace system
{

struct system_t::impl
{
    impl( system_t& self)
    {
        self.system_name_ = "Linux";

        // user name & home path
        struct passwd *p = getpwuid( geteuid());
        self.user_name_ = p->pw_name;
        self.home_path_ = boost::filesystem::path( p->pw_dir);

        // app user path
        {
            std::string dir_name( ".ramen");
            dir_name.append( RAMEN_VERSION_MAJOR_STR);
            self.application_user_path_ = self.home_path_ / dir_name;
        }

        // ram size
        {
            struct sysinfo info;
            sysinfo( &info);
            self.ram_size_ = info.totalram * info.mem_unit;
        }

        // executable location
        {
            char linkname[128];
            if( snprintf( linkname, sizeof( linkname), "/proc/%i/exe", getpid()) >= 0)
            {
                char buf[1024];
                int ret = readlink( linkname, buf, sizeof( buf));
                if( ret < sizeof( buf))
                {
                    buf[ret] = 0;
                    self.executable_path_ = boost::filesystem::path( buf);
                }
            }
        }
    }
};

} // system
} // ramen
