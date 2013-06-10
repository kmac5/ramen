// Copyright (c) 2013 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>
#include<pwd.h>
#include<libproc.h>
#include<sys/sysctl.h>

namespace ramen
{
namespace system
{

struct system_t::impl
{
    impl( system_t& self)
    {
        self.system_name_ = "OSX";

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
            int mib[2];
            mib[0] = CTL_HW;
            mib[1] = HW_MEMSIZE;
            int64_t size = 0;
            size_t len = sizeof( size);

            if( sysctl( mib, 2, &size, &len, NULL, 0) == 0)
                self.ram_size_ = size;
        }

        // executable location
        {
            char buf[PROC_PIDPATHINFO_MAXSIZE];
            if( proc_pidpath( getpid(), buf, sizeof( buf)) >= 0)
                self.executable_path_ = boost::filesystem::path( buf);
        }
    }
};

} // system
} // ramen
