// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_SHAPE_KEY_HPP
#define RAMEN_SHAPE_KEY_HPP

#include<vector>
#include<algorithm>

#include<adobe/copy_on_write.hpp>

#include<OpenEXR/ImathVec.h>

#include<ramen/assert.hpp>

#include<ramen/anim/keyframe.hpp>

#include<ramen/serialization/yaml_node.hpp>
#include<ramen/serialization/yaml_oarchive.hpp>

namespace ramen
{
namespace anim
{

template<class T>
class shape_key_t : public keyframe_t
{
public:

	typedef T point_type;
    typedef std::vector<T> value_type;

    shape_key_t() : keyframe_t()
	{
		set_v0_auto_tangent( keyframe_t::tangent_linear);
		set_v1_auto_tangent( keyframe_t::tangent_linear);
	}
	
    shape_key_t( float time, int num_pts = 0) : keyframe_t( time)
	{
		RAMEN_ASSERT( num_pts >= 0);

		set_v0_auto_tangent( keyframe_t::tangent_linear);
		set_v1_auto_tangent( keyframe_t::tangent_linear);
		
		if( num_pts)
			value().reserve( num_pts);
	}
	
	void swap( shape_key_t<T>& other)
	{
		keyframe_t::swap( other);
		std::swap( value_, other.value_);
	}
	
	void swap_value( shape_key_t<T>& other)
	{
		keyframe_t::swap_value( other);
		std::swap( value_, other.value_);
	}
	
	std::size_t size() const { return value().size();}
	bool empty() const { return value().empty();}
	
	typedef typename std::vector<T>::const_iterator const_iterator;
	typedef typename std::vector<T>::iterator		iterator;
	
    const std::vector<T>& value() const { return value_.read();}
    std::vector<T>& value()             { return value_.write();}

	const_iterator begin() const	{ return value().begin();}
	const_iterator end() const		{ return value().end();}

	iterator begin()	{ return value().begin();}
	iterator end()		{ return value().end();}

	void erase_points( int index, int num)
	{
		RAMEN_ASSERT( index >= 0 && index < size());
		value().erase( begin() + index, begin() + index + num);
	}
	
	void insert_points( int before, int num)
	{
		RAMEN_ASSERT( before >= 0 && before < size() - 1);
		value().insert( begin() + before, num, T());
	}
	
	void append_points( int num)
	{
		value().insert( end(), num, T());
	}
	
	// serialization
	void read( const serialization::yaml_node_t& node)
	{
		node.get_value( "time", time_);
		serialization::yaml_node_t points( node.get_node( "points"));
		value().reserve( points.size());
		
		for( int i = 0; i < points.size(); ++i)
		{
			point_type pt;
			points[i] >> pt;
			value().push_back( pt);
		}
	}

	void write( serialization::yaml_oarchive_t& out) const
	{
		out.begin_map();
			out << YAML::Key << "time" << YAML::Value << time();
			out << YAML::Key << "points" << YAML::Value;
				out.begin_seq();
					for( int i = 0; i < size(); ++i)
						out << value()[i];
				out.end_seq();
		out.end_map();
	}
	
private:

    // keyframes should be cheap to copy. Undo is currently based on it.
    // so we use adobe::copy_on_write for complex keyframes.
    adobe::copy_on_write<std::vector<T> > value_;
};

typedef shape_key_t<Imath::V2f> shape_key2f_t;
typedef shape_key_t<Imath::V3f> shape_key3f_t;

} // namespace
} // namespace

#endif
