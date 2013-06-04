// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/python/python.hpp>

#include<ramen/image/format.hpp>

#include<ramen/assert.hpp>

#include<ramen/app/application.hpp>

namespace ramen
{
namespace image
{

std::vector<format_t::preset_type> format_t::presets_;

format_t::format_t() : width( 768), height( 576), aspect( 1.0f) {}

format_t::format_t( int w, int h, float asp) : width( w), height( h), aspect( asp) {}

Imath::Box2i format_t::area() const { return Imath::Box2i( Imath::V2i( 0, 0), Imath::V2i( width, height));}

// presets
const std::vector<format_t::preset_type>& format_t::presets()
{
	init_presets();
	return presets_;
}

int format_t::preset_index() const
{
	for( int i = 0; i < presets().size(); ++i)
		if( *this == presets()[i].second)
			return i;

	return presets().size();
}

void format_t::set_from_preset( int index)
{
	RAMEN_ASSERT( index >= 0);
	RAMEN_ASSERT( index < presets().size());
	*this = presets()[index].second;
}

void format_t::set_default() { set_from_preset( 0);}

void format_t::init_presets()
{
	static bool inited = false;
	
	if( !inited)
	{
		// make a small list of presets inline.
		add_preset( preset_type( "PAL Square: 768 x 576"		, format_t( 768, 576, 1.0f)));
		add_preset( preset_type( "PAL D1: 720 x 576 x 1.067"	, format_t( 720, 576, 1.067f)));
		add_preset( preset_type( "PAL 16:9: 720 x 576 x 1.422"	, format_t( 720, 576, 1.422f)));
		
		add_preset( preset_type( "NTSC Square: 640 x 486"		, format_t( 640, 486, 1.0f)));
		add_preset( preset_type( "NTSC D1: 720 x 486 x 0.9"		, format_t( 720, 486, 0.9f)));
		add_preset( preset_type( "NTSC 16:9: 720 x 486 x 1.21"	, format_t( 720, 486, 1.21f)));

		add_preset( preset_type( "HD 720: 1280 x 720"	, format_t( 1280, 720, 1.0f)));
		add_preset( preset_type( "HD 1080: 1440 x 1080"	, format_t( 1440, 1080, 1.33f)));
		add_preset( preset_type( "HD 1080: 1920 x 1080"	, format_t( 1920, 1080, 1.0f)));
	
		add_preset( preset_type( "2K Super35: 2048 x 1556"			, format_t( 2048, 1556, 1.0f)));
		add_preset( preset_type( "2K Cinemascope: 1828 x 1556 x 2.0", format_t( 1828, 1556, 2.0f)));
			
		add_preset( preset_type( "4K Super35: 4096 x 3112"			, format_t( 4096, 3112, 1.0f)));
		add_preset( preset_type( "4K Cinemascope: 3656 x 3112 x 2.0", format_t( 3656, 3112, 2.0f)));
			
		add_preset( preset_type( "Cineon Full: 3656 x 2664"	, format_t( 3656, 2664, 1.0f)));
		inited = true;
	}
}

void format_t::add_preset( const preset_type& p)
{
	if( p.second.width > app().preferences().max_image_width())
		return;

	if( p.second.height > app().preferences().max_image_height())
		return;
	
	for( int i = 0; i < presets_.size(); ++i)
	{
		if( p == presets_[i])
			return;
	}

	presets_.push_back( p);
}

bool format_t::operator==( const format_t& other) const
{
    return width == other.width && height == other.height && aspect == other.aspect;
}

bool format_t::operator!=( const format_t& other) const
{
    return width != other.width || height != other.height || aspect != other.aspect;
}

// input & output
std::ostream& operator<<( std::ostream& os, const format_t& f)
{
    os << "[" << f.width << "," << f.height << "," << f.aspect << "]";
    return os;
}

void operator>>( const YAML::Node& in, format_t& f)
{
    in[0] >> f.width;
    in[1] >> f.height;
	in[2] >> f.aspect;
}

YAML::Emitter& operator<<( YAML::Emitter& out, const format_t& f)
{
    out << YAML::Flow << YAML::BeginSeq
		<< f.width << f.height << f.aspect;
	out << YAML::EndSeq;
    return out;
}

} // namespace
} // namespace
