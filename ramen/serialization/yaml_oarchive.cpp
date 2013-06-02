// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/serialization/yaml_oarchive.hpp>

#include<boost/filesystem/fstream.hpp>

#include<ramen/assert.hpp>

namespace ramen
{
namespace serialization
{

const int yaml_oarchive_t::version = 1;

yaml_oarchive_t::yaml_oarchive_t() : header_written_( false), map_level_( 0)
{
	begin_map();
}

void yaml_oarchive_t::write_composition_header()
{
	RAMEN_ASSERT( !header_written_);

	out_ << YAML::Key << "magic"	<< YAML::Value << "Ramen composition";
	out_ << YAML::Key << "version"	<< YAML::Value << version;
	check_errors();
	header_written_ = true;
}

void yaml_oarchive_t::write_to_file( const boost::filesystem::path& p)
{
	RAMEN_ASSERT( map_level() <= 1);

	if( map_level() == 1)
		end_map();

	boost::filesystem::ofstream ofs( p);

	if( !ofs.is_open() || !ofs.good())
		throw std::runtime_error( "Can't write to file");
	
	if( 1)
		ofs << out_.c_str();
	else
	{
		std::string buffer( out_.c_str(), out_.size());
		// encrypt & base 64 string
		ofs << buffer;
	}

	std::flush( ofs);
	ofs.close();
}

void yaml_oarchive_t::begin_map()
{
	out_ << YAML::BeginMap;
	++map_level_;
	check_errors();
}

void yaml_oarchive_t::end_map()
{
	RAMEN_ASSERT( map_level() > 0);

	out_ << YAML::EndMap;
	--map_level_;
	check_errors();
}

void yaml_oarchive_t::flow()
{
	out_ << YAML::Flow;
	check_errors();
}

void yaml_oarchive_t::begin_seq()
{
	out_ << YAML::BeginSeq;
	check_errors();
}

void yaml_oarchive_t::end_seq()
{
	out_ << YAML::EndSeq;
	check_errors();	
}

bool yaml_oarchive_t::good() const { return out_.good();}
void yaml_oarchive_t::check_errors() const { check_yaml_errors( out_);}
const std::string yaml_oarchive_t::last_error() const { return out_.GetLastError();}

} // namespace
} // namespace
