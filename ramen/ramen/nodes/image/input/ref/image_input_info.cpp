// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/params/image_input_info.hpp>

#include<adobe/algorithm/clamp.hpp>

#include<ramen/assert.hpp>

#include<ramen/serialization/yaml_oarchive.hpp>
#include<ramen/serialization/yaml_node.hpp>

namespace ramen
{

image_input_info_t::image_input_info_t()
{
    channels_ = tuple4i_t( 0, 1, 2, 3);
}

image_input_info_t::image_input_info_t( const filesystem::path_sequence_t& seq) : seq_( seq)
{
    channels_ = tuple4i_t( 0, 1, 2, 3);
}

image_input_info_t::image_input_info_t( const filesystem::path_sequence_t& seq,
										const std::vector<std::string>& channel_list) : seq_( seq), channel_list_( channel_list)
{
    channels_ = tuple4i_t( 0, 1, 2, 3);
}

boost::filesystem::path image_input_info_t::operator()( int n) const
{
	return seq_( adobe::clamp( n, seq_.start(), seq_.end()));
}

void image_input_info_t::read( const serialization::yaml_node_t& node)
{
	int version;
	node[0] >> version;
	
	std::string format;
	node[1] >> format;
	
	bool is_seq;
	node[2] >> is_seq;
	
	if( is_seq)
	{
		int start, end;
		node[3] >> start;
		node[4] >> end;
		set_path_sequence( filesystem::path_sequence_t( format, start, end));
		
		if( node.size() == 9)
		{
			int r, g, b, a;
			node[5] >> r;
			node[6] >> g;
			node[7] >> b;
			node[8] >> a;
			set_channels( r, g, b, a);
		}
	}
	else
	{
		set_path_sequence( filesystem::path_sequence_t( boost::filesystem::path( format), false));

		if( node.size() == 7)
		{
			int r, g, b, a;
			node[3] >> r;
			node[4] >> g;
			node[5] >> b;
			node[6] >> a;
			set_channels( r, g, b, a);
		}
	}
}

void image_input_info_t::write( serialization::yaml_oarchive_t& out) const
{
	out.flow();
	out.begin_seq();
		out << 1; // version
		out << YAML::DoubleQuoted << format_string();
		out << sequence().is_sequence();

		if( sequence().is_sequence())
		{
			out << sequence_start();
			out << sequence_end();
		}
			
		int r = boost::get<0>( channels());
		int g = boost::get<1>( channels());
		int b = boost::get<2>( channels());
		int a = boost::get<3>( channels());
			
		if( r != 0 && g != 1 && b != 2 && a != 3)
		{
			out << r;
			out << g;
			out << b;
			out << a;
		}
			
		out.end_seq();
}

bool image_input_info_t::operator==( const image_input_info_t& other) const
{
    return seq_ == other.seq_ && channels_ == other.channels_;
}

bool image_input_info_t::operator!=( const image_input_info_t& other) const
{
    return seq_ != other.seq_ || channels_ != other.channels_;
}

} // namespace
