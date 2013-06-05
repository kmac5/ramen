// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_ANIM_CURVE_HPP
#define	RAMEN_ANIM_CURVE_HPP

#include<string>

#include<boost/bind.hpp>
#include<boost/range/algorithm/for_each.hpp>
#include<boost/range/algorithm/remove_if.hpp>
#include<boost/range/algorithm/find_if.hpp>

#include<ramen/anim/keyframe_vector.hpp>

namespace ramen
{
namespace anim
{

enum extrapolation_method
{
	extrapolate_constant = 0,
	extrapolate_linear,
	extrapolate_repeat
};
	
template<class K>
class curve_t
{
public:

    typedef K key_type;
	typedef typename key_type::time_type time_type;
    
    typedef typename keyframe_vector_t<key_type>::const_iterator            const_iterator;
    typedef typename keyframe_vector_t<key_type>::iterator                  iterator;
    typedef typename keyframe_vector_t<key_type>::const_reverse_iterator    const_reverse_iterator;
    typedef typename keyframe_vector_t<key_type>::reverse_iterator          reverse_iterator;

    curve_t() { set_extrapolation( extrapolate_constant);}

    void swap( curve_t<K>& other)
	{ 
		keys().swap( other.keys());
		std::swap( extrap_, other.extrap_);
	}

    bool empty() const          { return keys().empty();}
    std::size_t size() const    { return keys().size();}

    void clear() { keys().clear();}

    const keyframe_vector_t<key_type>& keys() const { return keys_;}
    keyframe_vector_t<key_type>& keys()             { return keys_;}

    const key_type& operator[]( int x) const
    {
        assert( x >= 0 && x < size());
        return keys()[x];
    }

    key_type& operator[]( int x)
    {
        assert( x >= 0 && x < size());
        return keys()[x];
    }

    const_iterator begin() const    { return keys().begin();}
    const_iterator end() const      { return keys().end();}

    iterator begin()    { return keys().begin();}
    iterator end()      { return keys().end();}

    const_reverse_iterator rbegin() const    { return keys().rbegin();}
    const_reverse_iterator rend() const      { return keys().rend();}

    reverse_iterator rbegin()    { return keys().rbegin();}
    reverse_iterator rend()      { return keys().rend();}

	time_type start_time() const	{ return keys().front().time();}
	time_type end_time() const		{ return keys().back().time();}

	iterator insert( const key_type& k)
	{
		iterator it( keys().insert( k));	
		return it;
	}
	
	void erase( time_type time) { keys().erase( time);}
	
	void erase_selected_keyframes()
	{
        keys().erase( boost::range::remove_if( keys(), boost::bind( &key_type::selected, _1)), keys().end());
	}
	
	iterator iterator_from_keyframe_ptr( key_type *k)
	{
		for( iterator it( begin()); it != end(); ++it)
		{
			if( &(*it) == k)
				return it;
		}
	
		return end();
	}
	
    // extrapolation
    extrapolation_method extrapolation() const      { return extrap_;}
    void set_extrapolation( extrapolation_method m)	{ extrap_ = m;}

	// selection
	void select_all_keyframes( bool b) const
	{
        boost::range::for_each( keys(), boost::bind( &key_type::select, _1, b));
	}
	
	bool any_keyframe_selected() const
	{
        return boost::range::find_if( keys(), boost::bind( &key_type::selected, _1 )) != keys().end();
	}
	
	bool has_keyframe_at( time_type time) const { return keys().has_keyframe_at( time);}
	
protected:

    std::string extrapolation_method_to_string( extrapolation_method t) const
	{
		switch( t)
		{
			case extrapolate_constant:
				return "constant";
				
			case extrapolate_linear:
				return "linear";

			case extrapolate_repeat:
				return "repeat";
		}
	}

	extrapolation_method string_to_extrapolation_method( const std::string& s) const
	{
		if( s == "constant")
			return extrapolate_constant;
		
		if( s == "linear")
			return extrapolate_linear;
		
		if( s == "repeat")
			return extrapolate_repeat;
		
		assert( 0 && "Unknown extrapolation method");
	}
	
    keyframe_vector_t<key_type> keys_;
    extrapolation_method extrap_;
};

} // namespace
} // namespace

#endif
