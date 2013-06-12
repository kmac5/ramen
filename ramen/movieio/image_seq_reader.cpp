// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/movieio/image_seq_reader.hpp>

#include<boost/filesystem/operations.hpp>
#include<boost/lexical_cast.hpp>

#include<ramen/movieio/exceptions.hpp>

#include<ramen/imageio/factory.hpp>

#include<ramen/imageio/multichannel_reader.hpp>

namespace ramen
{
namespace movieio
{

image_seq_reader_t::image_seq_reader_t( const filesystem::path_sequence_t& seq) : seq_( seq)
{
	RAMEN_ASSERT( seq_.directory().is_absolute());

	init();	
}

image_seq_reader_t::image_seq_reader_t( const boost::filesystem::path& p, bool sequence)
{
	RAMEN_ASSERT( p.is_absolute());
	
	seq_ = filesystem::path_sequence_t( p, sequence);
	
	if( sequence)
	{
		boost::filesystem::directory_iterator it( p.parent_path());
		seq_.add_paths( it);
	}
	
	init();
}

void image_seq_reader_t::init()
{
	frame_ = seq_.start();
	has_extra_channels_ = false;
	get_sequence_info();
}

bool image_seq_reader_t::is_sequence() const			{ return seq_.is_sequence();}
int image_seq_reader_t::start_frame() const				{ return seq_.start();}
int image_seq_reader_t::end_frame() const				{ return seq_.end();}
int image_seq_reader_t::pad() const						{ return seq_.pad();}

std::string image_seq_reader_t::format_string() const
{ 
	std::string str = seq_.format_string();
	
	if( seq_.is_sequence())
	{
		str.append( 1, '[');
		str += boost::lexical_cast<std::string>( seq_.start());
		str.append( 1, '-');
		str += boost::lexical_cast<std::string>( seq_.end());
		str.append( 1, ']');
	}
	
	return str;
}

std::string image_seq_reader_t::string_for_current_frame() const
{
	return filesystem::hash_string( seq_( frame_));
}

void image_seq_reader_t::do_set_frame( int frame) { create_reader();}

void image_seq_reader_t::do_read_frame( const image::image_view_t& view,
                                        const math::box2i_t& crop,
                                        int subsample) const
{
	if( !reader_)
		throw movieio::unknown_movie_format();

	// translate exceptions
	try
	{
		reader_->read_image( view, crop, subsample);
	}
	catch( imageio::unknown_image_format& e)
	{
		throw unknown_movie_format();
	}
	catch( imageio::unsupported_image& e)
	{
		throw unsupported_movie( e.what());
	}
	catch( imageio::exception& e)
	{
		throw exception( e.what());
	}
}

void image_seq_reader_t::do_read_frame( const image::image_view_t& view,
                                        const math::box2i_t& crop,
                                        int subsample,
                                        const boost::tuple<int, int, int, int>& channels) const
{
	if( !reader_)
		throw movieio::unknown_movie_format();
	
	imageio::multichannel_reader_t *r = dynamic_cast<imageio::multichannel_reader_t*>( reader_.get());
	RAMEN_ASSERT( r);

	try
	{
		r->read_image( view, crop, subsample, channels);
	}
	catch( imageio::unknown_image_format& e)
	{
		throw unknown_movie_format();
	}
	catch( imageio::unsupported_image& e)
	{
		throw unsupported_movie( e.what());
	}
	catch( imageio::exception& e)
	{
		throw exception( e.what());
	}
}

void image_seq_reader_t::get_sequence_info()
{
	if( is_sequence())
	{
		for( int i = start_frame(); i <= end_frame(); ++i)
		{
			try
			{
                core::auto_ptr_t<imageio::reader_t> r( imageio::factory_t::instance().reader_for_image( seq_( i)));
				reader_.reset( r.release());
			
				if( reader_)
					break;
			}
			catch( ...)
			{
			}
		}
	}
	else
	{
		try
		{
            core::auto_ptr_t<imageio::reader_t> r( imageio::factory_t::instance().reader_for_image( seq_()));
			reader_.reset( r.release());
		}
		catch( ...)
		{
		}
	}
	
	if( reader_)
	{
		info_ = reader_->image_info();
		
		if( imageio::multichannel_reader_t *mreader = dynamic_cast<imageio::multichannel_reader_t*>( reader_.get()))
		{
			channels_ = mreader->channel_list();
			has_extra_channels_ = mreader->has_extra_channels();
		}
	}

	if( is_sequence())
		reader_.reset();
}

void image_seq_reader_t::create_reader()
{
	try
	{
        core::auto_ptr_t<imageio::reader_t> r( imageio::factory_t::instance().reader_for_image( seq_( frame_)));
		reader_.reset( r.release());
	}
	catch( imageio::exception& e)
	{
		reader_.reset();
        frame_info_ = core::dictionary_t();
		return;
	}

	frame_info_ = reader_->image_info();
	
	if( info_.empty())
	{
		info_ = reader_->image_info();
		
		if( imageio::multichannel_reader_t *mreader = dynamic_cast<imageio::multichannel_reader_t*>( reader_.get()))
		{
			channels_ = mreader->channel_list();
			has_extra_channels_ = mreader->has_extra_channels();
		}
	}
}

} // movieio
} // ramen
