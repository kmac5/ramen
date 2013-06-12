// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_SERIALIZATION_YAML_OARCHIVE_HPP
#define RAMEN_SERIALIZATION_YAML_OARCHIVE_HPP

#include<boost/noncopyable.hpp>

#include<ramen/serialization/yaml.hpp>

namespace ramen
{
namespace serialization
{

class RAMEN_API yaml_oarchive_t : boost::noncopyable
{
public:

	yaml_oarchive_t();

	void write_composition_header();
	bool header_written() const { return header_written_;}

	void write_to_file( const boost::filesystem::path& p);

	void begin_map();
	void end_map();

	void flow();
	void begin_seq();
	void end_seq();
	
	int map_level() const { return map_level_;}

	template<class T>
	yaml_oarchive_t& operator<<( const T& x)
	{
		out_ << x;
		check_errors();
		return *this;
	}

	YAML::Emitter& emitter() { return out_;}

	void check_errors() const;

private:

	bool good() const;
	const std::string last_error() const;


	YAML::Emitter out_;
	bool header_written_;
	int map_level_;

	static const int version;
};

} // namespace
} // namespace

#endif
