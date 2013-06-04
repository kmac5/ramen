// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_OFX_PLUGIN_MANAGER_HPP
#define RAMEN_OFX_PLUGIN_MANAGER_HPP

#include<boost/noncopyable.hpp>

#include<loki/Singleton.h>

#include"ofxCore.h"

#include"ofxhBinary.h"
#include"ofxhPropertySuite.h"
#include"ofxhClip.h"
#include"ofxhParam.h"
#include"ofxhMemory.h"
#include"ofxhImageEffect.h"
#include"ofxhPluginAPICache.h"
#include"ofxhPluginCache.h"
#include"ofxhHost.h"
#include"ofxhImageEffectAPI.h"

#include<ramen/filesystem/path.hpp>

#include<ramen/nodes/node.hpp>

namespace ramen
{
namespace ofx
{

class plugin_manager_impl : boost::noncopyable
{
public:

    struct plugin_info_t
    {
		std::string group;
		std::string label;
		std::string id;
    };

    typedef std::vector<plugin_info_t>::const_iterator const_iterator;
    const_iterator begin() const    { return plugins_info_.begin();}
    const_iterator end() const	    { return plugins_info_.end();}

    node_ptr_t create_node_by_id( std::string id, int major_version = -1, int minor_version = -1);
    node_ptr_t create_node_by_label( std::string label);

    OFX::Host::ImageEffect::ImageEffectPlugin *get_plugin_by_id( const std::string& id, int major = -1, int minor = -1);

private:

    friend struct Loki::CreateUsingNew<plugin_manager_impl>;

    plugin_manager_impl();
    ~plugin_manager_impl();

    boost::filesystem::path cache_path() const;

	node_ptr_t create_node( OFX::Host::ImageEffect::ImageEffectPlugin *plugin);
	
    OFX::Host::ImageEffect::PluginCache *image_effect_plugin_cache_;

    std::vector<plugin_info_t> plugins_info_;
};

typedef Loki::SingletonHolder<plugin_manager_impl> plugin_manager_t;

} // namespace
} // namespace

#endif
