// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/movieio/reader.hpp>

#include<boost/lexical_cast.hpp>

namespace ramen
{
namespace movieio
{

reader_t::reader_t() : frame_( 0) {}
reader_t::reader_t( const boost::filesystem::path& p) : p_( p), frame_( 0) {}

reader_t::~reader_t() {}

// sequence
bool reader_t::is_sequence() const { return true;}

int reader_t::pad() const { return 0;}

std::string reader_t::format_string() const { return filesystem::file_string( p_);}

std::string reader_t::string_for_current_frame() const
{
	std::string str( filesystem::hash_string( p_));

	if( is_sequence())
		return str + "-" + boost::lexical_cast<std::string>( frame_);

	return str;
}

// metadata
const core::dictionary_t& reader_t::movie_info() const { return info_;}

math::box2i_t reader_t::format() const
{
    return core::get<math::box2i_t>( movie_info(), core::name_t( "format"));
}

math::box2i_t reader_t::bounds() const
{
    math::box2i_t bounds( format());
    core::get<math::box2i_t>( movie_info(), core::name_t( "bounds"), bounds);
    return bounds;
}

float reader_t::aspect_ratio() const
{
	float aspect = 1.0f;
    core::get<float>( movie_info(), core::name_t( "aspect"), aspect);
	return aspect;
}

void reader_t::set_frame( int frame)
{
	if( is_sequence())
	{
		int start = start_frame();
		int end = end_frame();
		
		if( frame >= start && frame <= end)
		{
			frame_ = frame;
			do_set_frame( frame);
		}
		else
			throw frame_out_of_bounds( frame);		
	}
}

void reader_t::do_set_frame( int frame) {}

void reader_t::read_frame( const image::image_view_t& view) const
{
    read_frame( view, bounds(), 1);
}

void reader_t::read_frame( const image::image_view_t& view,
                           const math::box2i_t& crop,
                           int subsample) const
{ 
	do_read_frame( view, crop, subsample);
}

void reader_t::read_frame( const image::image_view_t& view,
                           const math::box2i_t& crop,
                           int subsample,
                           const boost::tuple<int, int, int, int>& channels) const
{
	do_read_frame( view, crop, subsample, channels);
}

void reader_t::do_read_frame( const image::image_view_t& view,
                              const math::box2i_t& crop,
                              int subsample,
                              const boost::tuple<int, int, int, int>& channels) const
{
	throw movieio::exception( "Movie format does not support multichannel reading");	
}

} // movieio
} // ramen
