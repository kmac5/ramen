// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_INPUT_CLIP_HPP
#define	RAMEN_IMAGE_INPUT_CLIP_HPP

#include<ramen/filesystem/path_sequence.hpp>

#include<ramen/movieio/reader.hpp>

namespace ramen
{
namespace image
{

class input_clip_t
{
public:

	input_clip_t();
	explicit input_clip_t( const filesystem::path_sequence_t& seq);
	
	void swap( input_clip_t& other);
	
	bool valid() const { return sequence_.valid();}
	
	const filesystem::path_sequence_t& path_sequence() const { return sequence_;}
	
	std::string format_string() const { return sequence_.format_string();}
	
    core::auto_ptr_t<movieio::reader_t> reader( const boost::filesystem::path& cwd) const;
	
	// paths
    void convert_relative_paths( const boost::filesystem::path& old_base,
                                 const boost::filesystem::path& new_base);

	void make_paths_absolute( const::boost::filesystem::path& from_dir);
	void make_paths_relative( const::boost::filesystem::path& from_dir);

private:

	filesystem::path_sequence_t sequence_;
};

} // image
} // ramen

#endif
