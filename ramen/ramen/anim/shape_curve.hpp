// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_ANIM_SHAPE_CURVE_HPP
#define	RAMEN_ANIM_SHAPE_CURVE_HPP

#include<OpenEXR/ImathBox.h>
#include<OpenEXR/ImathFun.h>

#include<boost/range/algorithm/copy.hpp>

#include<ramen/anim/shape_key.hpp>

#include<ramen/anim/curve.hpp>

#include<ramen/serialization/yaml_node.hpp>
#include<ramen/serialization/yaml_oarchive.hpp>

namespace ramen
{
namespace anim
{

template<class P>
class shape_curve_t : public curve_t<shape_key_t<P> >
{
public:

	typedef curve_t<shape_key_t<P> > superclass;

    typedef shape_key_t<P>					key_type;
    typedef typename key_type::time_type	time_type;
    typedef typename key_type::value_type	value_type;
	typedef typename key_type::point_type	point_type;

	typedef typename superclass::const_iterator const_iterator;
	typedef typename superclass::iterator		iterator;
	
    shape_curve_t() : curve_t<shape_key_t<P> >() {}
	
	void swap( shape_curve_t<P>& other) { superclass::swap( other);}
	
	key_type& insert( time_type t)
	{
		key_type k( t);
		
		if( !this->empty())
			k.value().reserve( this->begin()->size());
		
		iterator it( superclass::insert( k));
		return *it;
	}
	
	key_type& insert( const key_type& k)
	{
		iterator it( superclass::insert( k));
		return *it;
	}
	
	template<class OutIter>
	void evaluate( time_type time, OutIter out)
	{
		// handle extrapolation here, ...
		do_evaluate( time, out);
	}
	
	template<class OutIter>
	void do_evaluate( time_type time, OutIter out)
	{
		if( this->empty())
			return;
	
		if( time < this->start_time())
		{
			switch( this->extrapolation())
			{
				case extrapolate_constant:
					copy_keyframe( this->keys().front(), out);
				return;
	
				case extrapolate_linear:
				case extrapolate_repeat:
					assert( 0);
				return;
			}
		}

		if( time > this->end_time())
		{
			switch( this->extrapolation())
			{
				case extrapolate_constant:
					copy_keyframe( this->keys().back(), out);
				return;
	
				case extrapolate_linear:
				case extrapolate_repeat:
					assert( 0);
				return;
			}
		}
	
		const_iterator it( this->keys().lower_bound( time));
	
		if( time == it->time())
		{
			copy_keyframe( *it, out);
			return;
		}
	
		--it;
		
		if( it->v0_auto_tangent() == keyframe_t::tangent_step)
			copy_keyframe( *it, out);
		else
		{
			time_type t = ( time - it->time()) / ( (it+1)->time() - it->time());
			const value_type& v0( it->value());
			const value_type& v1( (it+1)->value());
			
			RAMEN_ASSERT( v0.size() == v1.size());
			
			for( int i = 0; i < v0.size(); ++i)
			{
				point_type q( Imath::lerp( v0[i], v1[i], t));
				*out = q;
				++out;
			}
		}
	}

	// Repeat for each keyframe.
	void erase_points( int index, int num)
	{
        boost::range::for_each( this->keys(), boost::bind( &key_type::erase_points, _1, index, num));
	}

	void insert_points( int before, int num)
	{
        boost::range::for_each( this->keys(), boost::bind( &key_type::insert_points, _1, before, num));
	}
	
	// serialization
	void read( const serialization::yaml_node_t& node)
	{
		// read extrapolation, ..., ... here.

		serialization::yaml_node_t keys_node( node.get_node( "keys"));
		
		for( int i = 0; i < keys_node.size(); ++i)
		{
			key_type k;
			k.read( keys_node[i]);
			this->keys().insert( k);
		}
	}

	void write( serialization::yaml_oarchive_t& out) const
	{
		out.begin_map();
			out << YAML::Key << "keys" << YAML::Value;
			out.begin_seq();
                boost::range::for_each( this->keys(), boost::bind( &key_type::write, _1, boost::ref( out)));
			out.end_seq();
		out.end_map();
	}
	
private:

	template<class OutIter>
    void copy_keyframe( const key_type& k, OutIter out) const { boost::range::copy( k.value(), out);}
};

typedef shape_curve_t<Imath::V2f> shape_curve2f_t;
typedef shape_curve_t<Imath::V3f> shape_curve3f_t;

} // namespace
} // namespace

#endif
