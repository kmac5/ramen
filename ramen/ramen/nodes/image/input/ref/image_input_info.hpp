// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_IMAGE_INPUT_INFO_HPP
#define	RAMEN_IMAGE_INPUT_INFO_HPP

#include<vector>

#include<boost/tuple/tuple.hpp>
#include<boost/tuple/tuple_io.hpp>
#include<boost/tuple/tuple_comparison.hpp>

#include<ramen/filesystem/path_sequence.hpp>

#include<ramen/serialization/archive_fwd.hpp>

namespace ramen
{

class image_input_info_t
{
public:

    typedef boost::tuple<int,int,int,int> tuple4i_t;

    image_input_info_t();
    image_input_info_t( const filesystem::path_sequence_t& seq);
    image_input_info_t( const filesystem::path_sequence_t& seq, const std::vector<std::string>& channel_list);

    bool valid() const { return seq_.valid();}

    boost::filesystem::path operator()( int n) const;

    const filesystem::path_sequence_t& sequence() const { return seq_;}
    filesystem::path_sequence_t& sequence()				{ return seq_;}
	
    void set_path_sequence( const filesystem::path_sequence_t& seq) { seq_ = seq;}

    int sequence_start() const  { return seq_.start();}
    int sequence_end() const	{ return seq_.end();}

    std::string format_string() const { return seq_.format_string();}

    const tuple4i_t& channels() const               { return channels_;}
    void set_channels( const tuple4i_t& c)          { channels_ = c;}
    void set_channels( int r, int g, int b, int a)  { channels_ = tuple4i_t( r, g, b, a);}
    void reset_channels()                           { channels_ = tuple4i_t( 0, 1, 2, 3);}

    const std::vector<std::string>& channel_list() const        { return channel_list_;}
    void set_channel_list( const std::vector<std::string>& c)	{ channel_list_ = c;}
    void clear_channel_list() { channel_list_.clear();}

	void read( const serialization::yaml_node_t& in);
    void write( serialization::yaml_oarchive_t& out) const;
	
    bool operator==( const image_input_info_t& other) const;
    bool operator!=( const image_input_info_t& other) const;

private:

    filesystem::path_sequence_t seq_;
    tuple4i_t channels_;
    std::vector<std::string> channel_list_;
};

} // namespace

#endif
