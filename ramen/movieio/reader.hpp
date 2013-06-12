// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_MOVIEIO_READER_HPP
#define	RAMEN_MOVIEIO_READER_HPP

#include<ramen/config.hpp>

#include<vector>
#include<string>

#include<boost/tuple/tuple.hpp>

#include<OpenEXR/ImathBox.h>

#include<ramen/assert.hpp>

#include<ramen/core/dictionary.hpp>
#include<ramen/core/memory.hpp>

#include<ramen/filesystem/path.hpp>

#include<ramen/image/typedefs.hpp>

#include<ramen/movieio/exceptions.hpp>

namespace ramen
{
namespace movieio
{

class RAMEN_API reader_t
{
public:

	reader_t();
    explicit reader_t( const boost::filesystem::path& p);
	
    virtual ~reader_t();
	
	// sequence
	virtual bool is_sequence() const;
	virtual int start_frame() const = 0;
	virtual int end_frame() const = 0;
	virtual int pad() const;
	virtual std::string format_string() const;
	virtual std::string string_for_current_frame() const;
	
	// metadata
    const core::dictionary_t& movie_info() const;
	
	// format
    math::box2i_t format() const;
    math::box2i_t bounds() const;
	float aspect_ratio() const;

	// channels
	bool is_multichannel() const							{ return !channel_list().empty();}
	virtual bool has_extra_channels() const					{ return false;}
	const std::vector<std::string>& channel_list() const	{ return channels_;}

	// time	
	void set_frame( int frame);
	
	// read
    void read_frame( const image::image_view_t& view) const;
    void read_frame( const image::image_view_t& view,
                     const math::box2i_t& crop,
                     int subsample) const;

    void read_frame( const image::image_view_t& view,
                     const math::box2i_t& crop,
                     int subsample,
                     const boost::tuple<int, int, int, int>& channels) const;
	
private:

    // non-copyable
    reader_t( const reader_t&);
    reader_t& operator=( const reader_t&);

	virtual void do_set_frame( int frame);
	
    virtual void do_read_frame( const image::image_view_t& view,
                                const math::box2i_t& crop,
                                int subsample) const = 0;

    virtual void do_read_frame( const image::image_view_t& view,
                                const math::box2i_t& crop,
                                int subsample,
                                const boost::tuple<int, int, int, int>& channels) const;
	
protected:
	
	boost::filesystem::path p_;
    core::dictionary_t info_;
	std::vector<std::string> channels_;
	int frame_;
};

} // movieio
} // ramen

#endif
