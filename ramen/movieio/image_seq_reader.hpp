// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_MOVIEIO_IMAGE_SEQ_READER_HPP
#define	RAMEN_MOVIEIO_IMAGE_SEQ_READER_HPP

#include<ramen/movieio/reader.hpp>

#include<boost/shared_ptr.hpp>

#include<ramen/filesystem/path_sequence.hpp>

#include<ramen/imageio/reader.hpp>

namespace ramen
{
namespace movieio
{

class RAMEN_API image_seq_reader_t : public reader_t
{
public:

	image_seq_reader_t( const boost::filesystem::path& p, bool sequence);
    explicit image_seq_reader_t( const filesystem::path_sequence_t& seq);
	
	virtual bool is_sequence() const;
	virtual int start_frame() const;
	virtual int end_frame() const;
	virtual int pad() const;

	virtual bool has_extra_channels() const	{ return has_extra_channels_;}
	
	virtual std::string format_string() const;
	virtual std::string string_for_current_frame() const;

private:

	void init();
	
	virtual void do_set_frame( int frame);
	
    virtual void do_read_frame( const image::image_view_t& view,
                                const math::box2i_t& crop,
                                int subsample) const;

    virtual void do_read_frame( const image::image_view_t& view,
                                const math::box2i_t& crop,
                                int subsample,
                                const boost::tuple<int, int, int, int>& channels) const;
	
	void get_sequence_info();
	
	void create_reader();
	
	filesystem::path_sequence_t seq_;
	boost::shared_ptr<imageio::reader_t> reader_;
    core::dictionary_t frame_info_;
	bool has_extra_channels_;
};

} // movieio
} // ramen

#endif
