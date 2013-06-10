// Copyright (c) 2010 Esteban Tovagliari.
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/app/preferences.hpp>

#include<stdlib.h>
#include<stdio.h>

#include<boost/filesystem/fstream.hpp>
#include<boost/filesystem/operations.hpp>
#include<boost/filesystem/convenience.hpp>

#include<ramen/app/application.hpp>

namespace ramen
{

preferences_t::preferences_t()
{
    set_defaults();
    load();
}

int preferences_t::max_image_width() const	{ return 16 * 1024;} // 16K
int preferences_t::max_image_height() const	{ return 16 * 1024;} // 16K

void preferences_t::set_defaults()
{
    max_image_memory_ = 30; // % of ram

    default_format_ = image::format_t();
    frame_rate_ = 25; // PAL by default.
		
	pick_distance_ = 5;
}

void preferences_t::load()
{
    boost::filesystem::path p = app().system().application_user_path() / "prefs.yaml";
    boost::filesystem::ifstream ifs( p);
    if( !ifs.is_open() || !ifs.good())
    {
        save();
        return;
    }

    try
    {
        YAML::Parser parser( ifs);

        YAML::Node doc;
        parser.GetNextDocument(doc);

        int version;

		if( !get_value( doc, "version", version))
            throw std::runtime_error( "Corrupted preferences file");

        get_value( doc, "max_image_memory", max_image_memory_);
		get_value( doc, "default_format", default_format_);
        get_value( doc, "frame_rate", frame_rate_);
		get_value( doc, "default_flipbook", flipbook_);
    }
    catch( ...)
    {
        set_defaults();
        save();
    }
}

void preferences_t::save()
{
    boost::filesystem::path p = app().system().application_user_path() / "prefs.yaml";
    boost::filesystem::ofstream ofs( p);

    if( !ofs.is_open() || !ofs.good())
    {
        app().error( "Couldn't open preferences file for writting. file = " + filesystem::file_string( p));
        return;
    }
	
    YAML::Emitter out;
    out << YAML::Comment( "Ramen preferences") << YAML::Newline;

    out << YAML::BeginMap;
        out << YAML::Key << "version" << YAML::Value << 1
            << YAML::Key << "max_image_memory" << YAML::Value << max_image_memory_
			<< YAML::Key << "default_format" << YAML::Value << default_format_
            << YAML::Key << "frame_rate" << YAML::Value << frame_rate_
            ;

    out << YAML::EndMap;
    ofs << out.c_str();
	std::flush( ofs);
    ofs.close();
}

void preferences_t::set_default_format( const image::format_t& format)
{
    default_format_ = format;
}

} // ramen
