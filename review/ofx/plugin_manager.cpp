// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/python/python.hpp>

#include<ramen/ofx/plugin_manager.hpp>

#include<fstream>

#include<ramen/ofx/host.hpp>
#include<ramen/ofx/ofx_node.hpp>

#include<ramen/system/system.hpp>

#include<ramen/app/plugin_manager.hpp>

#include<ramen/filesystem/path.hpp>

#include<iostream>

namespace ramen
{
namespace ofx
{

plugin_manager_impl::plugin_manager_impl()
{
	const std::vector<boost::filesystem::path> more_paths( ramen::plugin_manager_t::Instance().paths());
	
    OFX::Host::PluginCache::getPluginCache()->setCacheVersion( "RamenV1");
	
	for( int i = 0; i < more_paths.size(); ++i)
		OFX::Host::PluginCache::getPluginCache()->addFileToPath( filesystem::file_string( more_paths[i]));
	
    image_effect_plugin_cache_ = new OFX::Host::ImageEffect::PluginCache( ofx::host_t::Instance());
    image_effect_plugin_cache_->registerInCache( *OFX::Host::PluginCache::getPluginCache());

    // try to read an old cache
    std::ifstream ifs( filesystem::file_cstring( cache_path()));
    OFX::Host::PluginCache::getPluginCache()->readCache( ifs);
    OFX::Host::PluginCache::getPluginCache()->scanPluginFiles();
    ifs.close();

    // make our internal list
    std::map<std::string, OFX::Host::ImageEffect::ImageEffectPlugin *>::const_iterator it ( image_effect_plugin_cache_->getPluginsByID().begin());
    std::map<std::string, OFX::Host::ImageEffect::ImageEffectPlugin *>::const_iterator end( image_effect_plugin_cache_->getPluginsByID().end());

    for( ; it != end; ++it)
    {
		plugin_info_t info;
		info.group  = it->second->getDescriptor().getPluginGrouping();
		info.label  = it->second->getDescriptor().getLabel();
		info.id	    = it->first;
			
		if( !info.label.empty() && !info.group.empty())
			plugins_info_.push_back( info);
    }

    // save the plugin cache
    std::ofstream of( filesystem::file_cstring( cache_path()));
    OFX::Host::PluginCache::getPluginCache()->writePluginCache( of);
    of.close();
}

plugin_manager_impl::~plugin_manager_impl()
{
    // To avoid a crash at exit.
    // It should be safe the leak these objects.

    //OFX::Host::PluginCache::clearPluginCache();
    //delete image_effect_plugin_cache_;
}

boost::filesystem::path plugin_manager_impl::cache_path() const
{
    return system::system_t::Instance().preferences_path() / "RamenPluginCache.xml";
}

node_ptr_t plugin_manager_impl::create_node_by_id( std::string id, int major_version, int minor_version)
{
    OFX::Host::ImageEffect::ImageEffectPlugin* plugin = image_effect_plugin_cache_->getPluginById( id, major_version, minor_version);
	return create_node( plugin);
}

node_ptr_t plugin_manager_impl::create_node_by_label( std::string label)
{
    OFX::Host::ImageEffect::ImageEffectPlugin* plugin = image_effect_plugin_cache_->getPluginByLabel( label);
	return create_node( plugin);
}

node_ptr_t plugin_manager_impl::create_node( OFX::Host::ImageEffect::ImageEffectPlugin *plugin)
{
    if( plugin)
    {
		try
		{
			node_ptr_t n( new image::ofx_node_t( plugin));
			return n;
		}
		catch( ...)
		{
		    // we let each OFX plugin show it's own error
		}
    }

    return node_ptr_t();	
}

OFX::Host::ImageEffect::ImageEffectPlugin *plugin_manager_impl::get_plugin_by_id( const std::string& id, int major, int minor)
{
    return image_effect_plugin_cache_->getPluginById( id, major, minor);
}

} // namespace
} // namespace
