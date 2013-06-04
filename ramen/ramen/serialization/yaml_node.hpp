// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_SERIALIZATION_YAML_NODE_HPP
#define RAMEN_SERIALIZATION_YAML_NODE_HPP

#include<sstream>

#include<boost/optional.hpp>

#include<ramen/assert.hpp>

#include<ramen/serialization/yaml.hpp>

#include<ramen/serialization/archive_fwd.hpp>

namespace ramen
{
namespace serialization
{

class RAMEN_API yaml_node_t
{
public:

	yaml_node_t( yaml_iarchive_t *parent, const YAML::Node *node, int version = 1);
	yaml_node_t( const yaml_node_t& parent, const YAML::Node *node);
	
	int version() const	{ return version_;}

	// iterators	
	typedef YAML::Iterator const_iterator;	
	const_iterator begin() const	{ return node_->begin();}
	const_iterator end() const		{ return node_->end();}

	// indexing
	std::size_t size() const { return node_->size();}

	yaml_node_t operator [] ( int index) const
	{
		RAMEN_ASSERT( index >= 0 && index < size());
		return yaml_node_t( parent_, &(*node_)[index], version());
	}

	// stream
	template<class T>
	void operator>>( T& value) const
	{
		// TODO: check for errors here!
		(*node_) >> value;
	}
	
	// values
    template<class T>
    void get_value( const std::string& key, T& value) const
    {
		(*node_)[key] >> value;
    }

    template<class T>
    bool get_optional_value( const std::string& key, T& value) const
    {
        if( const YAML::Node *n = node_->FindValue( key))
        {
			try
			{
	            *n >> value;
	            return true;
			}
			catch( YAML::Exception& e) {}
        }

        return false;
    }
	
	// nodes
	yaml_node_t get_node( const std::string& key) const;
	boost::optional<yaml_node_t> get_optional_node( const std::string& key) const;
	
	const YAML::Node *yaml_node() const { return node_;}
	
	// errors
	std::stringstream& error_stream() const;
	
private:

	friend class yaml_iarchive_t;
	
	void set_version( int v) const;
	
	yaml_iarchive_t *parent_;
	const YAML::Node *node_;
	mutable int version_;
};

typedef boost::optional<yaml_node_t> optional_yaml_node_t;

} // namespace
} // namespace

#endif
