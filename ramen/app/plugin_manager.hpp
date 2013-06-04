// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_PLUGIN_MANAGER_HPP
#define RAMEN_PLUGIN_MANAGER_HPP

#include<set>

#include<boost/noncopyable.hpp>
#include<boost/ptr_container/ptr_vector.hpp>

#include<ramen/filesystem/path.hpp>

#include<ramen/rdk/rdk.hpp>

namespace ramen
{

class plugin_manager_t : boost::noncopyable
{
public:

    static plugin_manager_t& instance();

	const std::vector<boost::filesystem::path>& paths() const { return paths_;}
	
private:

    plugin_manager_t();
    ~plugin_manager_t();

    void load_plugins_in_directory( const boost::filesystem::path& p);
    void load_plugin( const boost::filesystem::path& p);

	std::set<std::string> plugin_names_;
    //boost::ptr_vector<boost::extensions::shared_library> plugins_;
	
	std::vector<boost::filesystem::path> paths_;
	rdk_app_info_t app_info_;
};

} // namespace

#endif
