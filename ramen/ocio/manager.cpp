// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/ocio/manager.hpp>

#include<cstdlib>

#include<boost/filesystem/operations.hpp>

#include<ramen/assert.hpp>

#include<ramen/app/application.hpp>

#include<iostream>

namespace ramen
{
namespace ocio
{

manager_t::manager_t()
{
	init();
	get_displays();
}

manager_t::~manager_t() {}

void manager_t::init()
{
	if( getenv( "OCIO"))
	{
		try
		{
		    OCIO::ConstConfigRcPtr config = OCIO::GetCurrentConfig();
			return;
		}
		catch( OCIO::Exception & exception)
		{
			#ifndef NDEBUG
				std::cerr << "Couldn't read $OCIO config. Trying Ramen's default config.";
			#endif
		}
	}

	// try more paths here...
	boost::filesystem::path ocio_path = app().system().app_user_path() / "ocio/config.ocio";

	if( init_from_file( ocio_path))
		return;

	ocio_path = app().system().app_bundle_path() / "ocio/config.ocio";

	if( init_from_file( ocio_path))
		return;

	// Fallback
	OCIO::ConstConfigRcPtr config = OCIO::GetCurrentConfig();
}

OCIO::ConstConfigRcPtr manager_t::config() const
{
	OCIO::ConstConfigRcPtr config = OCIO::GetCurrentConfig();
	config->sanityCheck();
	return config;
}

bool manager_t::init_from_file( const boost::filesystem::path& p)
{
	if( !boost::filesystem::exists( p))
		return false;

	try
	{
		OCIO::ConstConfigRcPtr config = OCIO::Config::CreateFromFile( filesystem::file_string( p).c_str());
		OCIO::SetCurrentConfig( config);
		return true;
	}
	catch( ...)
	{
		return false;
	}
}

void manager_t::get_displays()
{
	OCIO::ConstConfigRcPtr cfg = config();
    default_display_ = cfg->getDefaultDisplay();
    int num_device_names = cfg->getNumDisplays();

    for(int i = 0; i < num_device_names; i++)
    {
        std::string dispname = cfg->getDisplay( i);
        displays_.push_back( dispname);

        if( default_display_ == dispname)
            default_display_index_ = i;
    }
}

void manager_t::get_views( const std::string& display, std::vector<std::string>& views, int& default_index) const
{
    OCIO::ConstConfigRcPtr cfg = config();
    int num_views = cfg->getNumViews( display.c_str());
    std::string default_view = cfg->getDefaultView( display.c_str());
	default_index = 0;
	views.clear();

    for( int i = 0; i < num_views; ++i)
    {
        std::string vname = cfg->getView( display.c_str(), i);
        views.push_back( vname);

        if( default_view == vname)
            default_index = i;
    }
}

std::string manager_t::default_view( const std::string& display) const
{
    OCIO::ConstConfigRcPtr cfg = config();
	return cfg->getDefaultView( display.c_str());
}

OCIO::ConstContextRcPtr manager_t::get_context( const context_t *ctx) const
{
	OCIO::ConstConfigRcPtr cfg = config();
	OCIO::ConstContextRcPtr context = cfg->getCurrentContext();

	if( !ctx)
		return context;

    OCIO::ContextRcPtr mutable_context;

	for( int i = 0; i < ctx->size(); ++i)
	{
		std::pair<std::string, std::string> ctx_pair = (*ctx)[i];

	    if( !ctx_pair.first.empty())
		{
	        if( !mutable_context)
				mutable_context = context->createEditableCopy();

			mutable_context->setStringVar( ctx_pair.first.c_str(), ctx_pair.second.c_str());
	    }
	}

    if( mutable_context)
		return mutable_context;

	return context;
}

} // ocio
} // ramen
