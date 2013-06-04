// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_PREFERENCES_HPP
#define RAMEN_PREFERENCES_HPP

#include<ramen/python/python.hpp>

#include<vector>
#include<string>

#include<boost/noncopyable.hpp>

#include<OpenEXR/ImfChromaticities.h>

#include<ramen/app/application_fwd.hpp>

#include<ramen/filesystem/path.hpp>

#include<ramen/image/format.hpp>

#include<ramen/serialization/yaml.hpp>

namespace ramen
{

/**
\ingroup app
\brief contains the user preferences
*/
class preferences_t : boost::noncopyable
{
public:

    // memory
    int max_image_memory() const        { return max_image_memory_;}
    void set_max_image_memory( int m)   { max_image_memory_ = m;}
	
    // format
    void set_default_format( const image::format_t& format);
    image::format_t default_format() const { return default_format_;}

    int frame_rate() const      { return frame_rate_;}
    void set_frame_rate( int f) { frame_rate_ = f;}

	// image limits
	int max_image_width() const;
	int max_image_height() const;
	
	// paths
	const boost::filesystem::path& tmp_dir() const;
	
	// flip
	const std::string& default_flipbook()  const;
	void set_default_flipbook( const std::string& f);
	
	// user interface
	int pick_distance() const { return pick_distance_;}

    void save();

private:

    friend class application_t;

    preferences_t();

    void set_defaults();

    void load();

    template<class T>
    bool get_value( const YAML::Node& doc, const std::string& key, T& value)
    {
        if( const YAML::Node *n = doc.FindValue( key))
        {
            *n >> value;
            return true;
        }

        return false;
    }

    image::format_t default_format_;
    int frame_rate_;
    int max_image_memory_;
	boost::filesystem::path tmp_dir_;
	std::string flipbook_;
	int pick_distance_;
};

} // namespace

#endif
