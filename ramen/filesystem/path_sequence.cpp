// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/filesystem/path_sequence.hpp>

#include<algorithm>

#include<boost/tokenizer.hpp>
#include<boost/lexical_cast.hpp>

#include<ramen/assert.hpp>

namespace ramen
{
namespace filesystem
{
	
path_sequence_t::path_sequence_t()
{
    start_ = end_ = pad_ = 0;
    is_seq_ = false;
	valid_ = false;
}

path_sequence_t::path_sequence_t( const boost::filesystem::path& p, bool sequence)
{
	valid_ = true;	
    dir_ = p.parent_path();
    std::string str = p.filename().string();

    boost::tuple<std::string, std::string, std::string> parts( decompose_filename( str));

    if( sequence)
    {
		base_ = boost::get<0>( parts);
		ext_ = boost::get<2>( parts);
	
		start_ = end_ = 0;
		pad_ = 1;
	
		std::string fnum = boost::get<1>( parts);
		is_seq_ = !fnum.empty();

		if( is_seq_)
		{
		    pad_ = get_pad( fnum);
		    start_ = boost::lexical_cast<int>( fnum);
		    end_ = start_;
		}
    }
    else
    {
		is_seq_ = false;
		ext_ = boost::get<2>( parts);
		base_ = std::string( str, 0, str.size() - ext_.size());
    	start_ = end_ = 0;
		pad_ = 1;
    }
}

path_sequence_t::path_sequence_t( const std::string& format, int start, int end)
{
	//TODO: add error checking here.
	
	valid_ = true;
	
	std::string part;
	
	std::size_t pos = format.find_last_of( '/');
	
	if( pos != std::string::npos)
	{
		dir_ = boost::filesystem::path( std::string( format, 0, pos));
		part = std::string( format, pos + 1, std::string::npos);
	}
	else
		part = format;
	
	pos = part.find_last_of( '.');

	if( pos != std::string::npos)
	{
		ext_ = std::string( part, pos, std::string::npos);
		part = std::string( part, 0, pos);
	}
	
	pos = part.find_last_of( '%');

	if( pos != std::string::npos)
	{
		is_seq_ = true;
		start_ = start;
		end_ = end;
		pad_ = 0;
		
		if( pos != 0)
		{
			base_ = std::string( part, 0, pos);
			part = std::string( part, pos, std::string::npos);
		}
		
		if( part.size() != 2)
		{
			std::string pad_str( part, 1, part.size() - 2);
			pad_ = boost::lexical_cast<int>( pad_str);
		}
	}
	else
	{
		base_ = part;
		is_seq_ = false;
	}
}

void path_sequence_t::swap( path_sequence_t& other)
{
	std::swap( valid_, other.valid_);
	std::swap( dir_, other.dir_);
	std::swap( base_, other.base_);
	std::swap( start_, other.start_);
	std::swap( end_, other.end_);
	std::swap( pad_, other.pad_);
	std::swap( ext_, other.ext_);
	std::swap( is_seq_, other.is_seq_);
}

boost::filesystem::path path_sequence_t::operator()() const
{
	RAMEN_ASSERT( valid());
	RAMEN_ASSERT( !is_sequence());

	return dir_ / ( base_ + ext_);
}

boost::filesystem::path path_sequence_t::operator()( int n) const
{
	RAMEN_ASSERT( valid());
	
    if( !is_sequence())
        return dir_ / ( base_ + ext_);

    std::string num_str = boost::lexical_cast<std::string>( n);

    while( num_str.length() < pad_)
            num_str.insert( 0, "0");

    std::string filename = base_ + num_str + ext_;
    return dir_ / filename;
}

std::string path_sequence_t::format_string() const
{
	RAMEN_ASSERT( valid());

	std::string part;
	
	if( dir_.empty())
		part = std::string( "./") + base_;
	else
		part = file_string( dir_) + "/" + base_;
	
    if( is_seq_)
    {
        std::string fnum;

        if( pad_ != 1)
        {
            fnum = "%0";
            fnum += boost::lexical_cast<std::string>( pad_);
            fnum += "d";
        }
        else
            fnum = "%d";

        return part + fnum + ext_;
    }
	else
	{
		if( base_.empty())
			return std::string();
		
		return part + ext_;
	}
}

bool path_sequence_t::add_path( const boost::filesystem::path& p)
{
	RAMEN_ASSERT( valid());
	
    if( !is_seq_)
        return false;

    std::string str = p.filename().string();
    boost::tuple<std::string, std::string, std::string> parts( decompose_filename( str));

    std::string new_base = boost::get<0>( parts);

    if( base_.empty())
    {
		if( !new_base.empty())
			return false;
    }
    else
    {
		if( base_ != new_base)
			return false;
    }
    
    std::string new_ext = boost::get<2>( parts);
    
    if( ext_ != new_ext)
		return false;

    std::string fnum( boost::get<1>( parts));

    if( fnum.empty())
		return false;

    int n = boost::lexical_cast<int>( fnum);
    start_ = std::min( start_, n);
    end_ = std::max( end_, n);

    pad_ = std::max( pad_, get_pad( fnum));
    return true;
}

void path_sequence_t::add_paths( boost::filesystem::directory_iterator itr)
{
	RAMEN_ASSERT( valid());
	
    for( ; itr != boost::filesystem::directory_iterator(); ++itr)
    {
		if( !boost::filesystem::is_directory( itr->path()))
		{
			if( itr->path().filename().string()[0] != '.')
				add_path( itr->path());
		}
    }	
}

std::string path_sequence_t::get_extension( const std::string& str) const
{
    std::string::size_type n = str.find_last_of( ".");

    if( n != std::string::npos)
    {
		std::string ext( std::string( str, n, std::string::npos));

    	if( !ext.empty())
    	{
		    for( int i=1;i<ext.length();++i)
		    {
				if( !isdigit( ext[i]))
				    return ext;
		    }
		}
    }

    return std::string();
}

std::string path_sequence_t::get_frame_number( const std::string& str) const
{
    try
    {
		// this is a quick fix, if all str are nums
		int num = boost::lexical_cast<int>( str);
		return str;
    }
    catch( ...) {}

    std::string::size_type n = str.length() - 1;

    while( n != 0)
    {
		char c = str[n];
	
		if( !isdigit( c))
			break;
	
		--n;
    }

    return std::string( str, n + 1, str.length());
}

int path_sequence_t::get_pad( const std::string& str) const
{
    if( str[0] == '0')
		return str.length();

    return 1;
}

boost::tuple<std::string, std::string, std::string> path_sequence_t::decompose_filename( const std::string& filename) const
{
    std::string ext  = get_extension( filename);
    std::string fnum = get_frame_number( std::string( filename, 0, filename.length() - ext.length()));

    std::string name;
    int name_lenght = filename.length() - fnum.length() - ext.length();

    if( name_lenght > 0)
		name = std::string( filename, 0, name_lenght);

    return boost::make_tuple( name, fnum, ext);
}

void path_sequence_t::convert_relative_paths( const boost::filesystem::path& old_base, const boost::filesystem::path& new_base)
{
	RAMEN_ASSERT( old_base.is_absolute());
	RAMEN_ASSERT( new_base.is_absolute());
	
	if( directory().is_relative())
	{
		boost::filesystem::path new_dir = filesystem::convert_relative_path( directory(), old_base, new_base);
		set_directory( new_dir);
	}
}

void path_sequence_t::make_paths_absolute( const boost::filesystem::path& from_dir)
{	
	RAMEN_ASSERT( from_dir.is_absolute());
	
	if( directory().is_relative())
	{
		boost::filesystem::path new_dir = filesystem::make_absolute_path( directory(), from_dir);		
		set_directory( new_dir);
	}
}

void path_sequence_t::make_paths_relative( const boost::filesystem::path& from_dir)
{
	RAMEN_ASSERT( from_dir.is_absolute());

	if( directory().is_absolute())
	{
		boost::filesystem::path new_dir = filesystem::make_relative_path( directory(), from_dir);
		set_directory( new_dir);
	}
}

void operator>>( const YAML::Node& in, path_sequence_t& seq)
{
	switch( in.size())
	{
	case 0:
		seq = path_sequence_t();
	break;
		
	case 1:
	{
		std::string p;
		in[0] >> p;
		seq = path_sequence_t( boost::filesystem::path( p), false);
	}
	break;
		
	case 3:
	{
		std::string format;
		int start, end;
		
		in[0] >> format;
		in[1] >> start;
		in[2] >> end;
		seq = path_sequence_t( format, start, end);
	}
	break;
	
	default:
		RAMEN_ASSERT( 0);
	};
}

YAML::Emitter& operator<<( YAML::Emitter& out, const path_sequence_t& seq)
{
    out << YAML::Flow << YAML::BeginSeq;

	if( seq.valid())
	{
		out << YAML::DoubleQuoted << seq.format_string();
		
		if( seq.is_sequence())
		{
			out << seq.start();
			out << seq.end();
		}
	}
	
	out << YAML::EndSeq;
    return out;
}

} // namespace
} // namespace
