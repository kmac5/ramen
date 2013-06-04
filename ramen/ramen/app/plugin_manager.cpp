// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/python/python.hpp>

#include<ramen/app/plugin_manager.hpp>

#include<boost/foreach.hpp>
#include<boost/function.hpp>
#include<boost/filesystem/operations.hpp>

#include<ramen/app/application.hpp>

#include<iostream>

#include<dlfcn.h>

namespace ramen
{

plugin_manager_t& plugin_manager_t::instance()
{
    static plugin_manager_t p;
    return p;
}

plugin_manager_t::plugin_manager_t()
{	
    boost::filesystem::path plugins_path = app().system().app_bundle_path();
    plugins_path /= "plugins";
	paths_.push_back( plugins_path);
		
	plugins_path = app().system().home_path() / "ramen/plugins";
	paths_.push_back( plugins_path);
	
	for( int i = 0; i < paths_.size(); ++i)
		load_plugins_in_directory( paths_[i]);
}

plugin_manager_t::~plugin_manager_t()
{
    /*
    BOOST_FOREACH( boost::extensions::shared_library& lib, plugins_)
    {
        // call a cleanup function for each plugin here
        boost::function<void()> f = lib.get<void>( "cleanup_ramen_plugin");

        if( f)
            f();
    }
    */
}

void plugin_manager_t::load_plugins_in_directory( const boost::filesystem::path& p)
{
    // TODO: error handling, recurse

    if( !boost::filesystem::exists( p))
        return;

    boost::filesystem::directory_iterator end_iter;
    for( boost::filesystem::directory_iterator dir_itr( p); dir_itr != end_iter; ++dir_itr)
    {
        if( boost::filesystem::is_regular_file( dir_itr->status()))
            load_plugin( *dir_itr);
        else
        {
            if( boost::filesystem::is_directory( dir_itr->status()))
				load_plugins_in_directory( *dir_itr);
        }
    }
}

void plugin_manager_t::load_plugin( const boost::filesystem::path& p)
{
    // TODO: add more error handling.
	
    if( p.extension() == ".so")
    {
		std::string fname = p.filename().string();

		if( plugin_names_.find( fname) != plugin_names_.end())
			return;

        /*
        std::auto_ptr<boost::extensions::shared_library> lib( new boost::extensions::shared_library( filesystem::file_string( p), true));

        if( lib->open())
        {
            boost::function<rdk_error_t( const rdk_app_info_t&)> f = lib->get<rdk_error_t, const rdk_app_info_t&>( "register_ramen_plugin");
            
            if( f)
            {
                rdk_error_t err = f( app_info_);
				
				if( err == rdk_no_err)
				{
	                plugins_.push_back( lib);
					plugin_names_.insert( fname);
	                return;
				}
				else
					std::cerr << "error while loading plugin " << filesystem::file_string( p) << "\n";
            }
            else
                std::cerr << "couldn't find register function in plugin " << filesystem::file_string( p) << "\n";
        }
        else
		{
            std::cerr << "couldn't open plugin " << filesystem::file_string( p) << ".\n";
			std::cerr << "dlerr = " << dlerror() << "\n\n";
		}
        */
    }
}

} // namespace
