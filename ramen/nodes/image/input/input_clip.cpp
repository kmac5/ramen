// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/python/python.hpp>

#include<stdio.h>

#include<algorithm>

#include<ramen/nodes/image/input/input_clip.hpp>

#include<ramen/movieio/factory.hpp>

namespace ramen
{
namespace image
{

input_clip_t::input_clip_t() {}
input_clip_t::input_clip_t( const filesystem::path_sequence_t& seq) : sequence_( seq) {}

void input_clip_t::swap( input_clip_t& other)
{
	sequence_.swap( other.sequence_);
}

std::auto_ptr<movieio::reader_t> input_clip_t::reader( const boost::filesystem::path& cwd) const
{
	if( valid())
	{
		filesystem::path_sequence_t new_seq = sequence_;
		
		if( new_seq.directory().is_relative())
			new_seq.make_paths_absolute( cwd);
		
		return movieio::factory_t::instance().create_reader( new_seq);
	}
	
	return std::auto_ptr<movieio::reader_t>();
}

// paths
void input_clip_t::convert_relative_paths( const boost::filesystem::path& old_base, const boost::filesystem::path& new_base)
{
	if( valid())
		sequence_.convert_relative_paths( old_base, new_base);
}

void input_clip_t::make_paths_absolute( const::boost::filesystem::path& from_dir)
{
	if( valid())
		sequence_.make_paths_absolute( from_dir);
}

void input_clip_t::make_paths_relative( const::boost::filesystem::path& from_dir)
{
	if( valid())
		sequence_.make_paths_relative( from_dir);
}

} // namespace
} // namespace

