// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_FORMAT_HPP
#define RAMEN_IMAGE_FORMAT_HPP

#include<vector>
#include<string>
#include<utility>

#include<OpenEXR/ImathBox.h>

#include<ramen/filesystem/path.hpp>

#include<ramen/serialization/yaml.hpp>

namespace ramen
{
namespace image
{

struct format_t
{
    format_t();
    format_t( int w, int h, float asp = 1.0f);

    Imath::Box2i area() const;

	// presets
	typedef std::pair<std::string, format_t> preset_type;

	static const std::vector<preset_type>& presets();

	int preset_index() const;
	void set_from_preset( int index);

	void set_default();

	// operators
    bool operator==( const format_t& other) const;
    bool operator!=( const format_t& other) const;

	static void init_presets();
	static void add_preset( const preset_type& p);
	
	// data
    int width, height;
	float aspect;
	
private:

	static std::vector<preset_type> presets_;
};

std::ostream& operator<<( std::ostream& os, const format_t& f);

void operator>>( const YAML::Node& in, format_t& f);
YAML::Emitter& operator<<( YAML::Emitter& out, const format_t& f);

} // namespace
} // namespace

#endif
