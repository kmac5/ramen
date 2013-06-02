// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_MOTION_BLUR_INFO_HPP
#define RAMEN_MOTION_BLUR_INFO_HPP

#include<ramen/config.hpp>

#include<ramen/serialization/yaml.hpp>

namespace ramen
{

struct RAMEN_API motion_blur_info_t
{
    enum filter_type
    {
        box_filter = 0,
        triangle_filter,
        cubic_filter
    };

    motion_blur_info_t() : shutter( 0), samples( 1), shutter_offset(0), filter( box_filter) {}

    bool operator==( const motion_blur_info_t& other) const;
    bool operator!=( const motion_blur_info_t& other) const;

    int samples;
    float shutter;
    float shutter_offset;
    filter_type filter;
	
    struct loop_data_t
    {
        loop_data_t( float time, int samples = 1, float shutter = 0, float offset = 0,
					 motion_blur_info_t::filter_type f = motion_blur_info_t::box_filter);

        float weight_for_time( float t) const;

        int num_samples;
        float start_time;
        float center_time;
        float end_time;
        float time_step;
        filter_type filter;
    };
	
	std::string filter_to_string( filter_type f) const;
	filter_type string_to_filter( const std::string& s) const;
};

void operator>>( const YAML::Node& in, motion_blur_info_t& f);
YAML::Emitter& operator<<( YAML::Emitter& out, const motion_blur_info_t& f);

} // namespace

#endif
