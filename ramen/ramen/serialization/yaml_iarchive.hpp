// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_SERIALIZATION_YAML_IARCHIVE_HPP
#define RAMEN_SERIALIZATION_YAML_IARCHIVE_HPP

#include<utility>
#include<sstream>

#include<boost/noncopyable.hpp>

#include<ramen/serialization/yaml_node.hpp>

namespace ramen
{
namespace serialization
{

class yaml_iarchive_t : boost::noncopyable
{
public:

	explicit yaml_iarchive_t( std::istream& is);

	const yaml_node_t& root() const;

	bool read_composition_header();
	bool header_read() const;

	int version() const { return version_;}

	template<class T>
	void get_value( const std::string& key, T& value) const { root().get_value( key, value);}

	yaml_node_t get_node( const std::string& key) const;

	// optional values
	template<class T>
	bool get_optional_value( const std::string& key, T& value) const
	{
		return root().get_optional_value( key, value);
	}

	boost::optional<yaml_node_t> get_optional_node( const std::string& key) const;

	// errors
	std::stringstream& error_stream() { return error_stream_;}
	std::string errors() const;
	
	// util
	static std::ios_base::openmode file_open_mode();
	
private:

	void init( std::istream& is);
	
	YAML::Parser parser_;
	YAML::Node doc_;
	std::auto_ptr<yaml_node_t> root_;
	
	int version_;
	bool header_read_;
	
	std::stringstream error_stream_;
};

} // namespace
} // namespace

#endif
