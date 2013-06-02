// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_ANIM_KEYFRAME_VECTOR_HPP
#define RAMEN_ANIM_KEYFRAME_VECTOR_HPP

#include<vector>
#include<iostream>

#include<boost/optional.hpp>

#include<ramen/anim/keyframe.hpp>

namespace ramen
{
namespace anim
{
namespace
{

template<class T>
struct keyframe_less
{
    typedef bool result_type;

    template<class K>
    bool operator()( const K& a, const K& b) const  { return a.time() < b.time();}

    template<class K>
    bool operator()( const K& a, T b) const     { return a.time() < b;}

    // MSVC debug builds requires the next overloads
    template<class K>
    bool operator()( T a, const K& b) const     { return a < b.time();}

    bool operator()( T a, T b) const        { return a < b;}
};

template<class T>
T abs( T x)
{
	if( x < 0)
		return -x;
	
	return x;
}

} // namespace

template<class K>
class keyframe_vector_t
{
public:

    typedef K key_type;
    typedef typename K::value_type	value_type;
	typedef typename K::time_type	time_type;

    keyframe_vector_t() {}

    void swap( keyframe_vector_t<key_type>& other) { keys().swap( other.keys());}

    bool empty() const          { return keys().empty();}
    std::size_t size() const    { return keys().size();}

    void clear() { keys().clear();}

    const std::vector<key_type>& keys() const	{ return keys_;}
    std::vector<key_type>& keys()               { return keys_;}

    key_type& operator[]( std::size_t i)
    {
        assert( i >= 0 && i < size());
        return keys()[i];
    }

    const key_type& operator[]( std::size_t i) const
    {
        assert( i >= 0 && i < size());
        return keys()[i];
    }

    typedef typename std::vector<key_type>::const_iterator const_iterator;
    typedef typename std::vector<key_type>::iterator       iterator;

    const_iterator begin() const	{ return keys().begin();}
    const_iterator end() const		{ return keys().end();}

    iterator begin()	{ return keys().begin();}
    iterator end()      { return keys().end();}

    typedef typename std::vector<key_type>::const_reverse_iterator const_reverse_iterator;
    typedef typename std::vector<key_type>::reverse_iterator       reverse_iterator;

    const_reverse_iterator rbegin() const	{ return keys().rbegin();}
    const_reverse_iterator rend() const		{ return keys().rend();}

    reverse_iterator rbegin()	{ return keys().rbegin();}
    reverse_iterator rend()		{ return keys().rend();}

    const key_type& front() const	{ return keys().front();}
    key_type& front()               { return keys().front();}

    const key_type& back() const	{ return keys().back();}
    key_type& back()                { return keys().back();}

    iterator insert( const key_type& key)
    {
        if( empty())
        {
            keys().push_back( key);
            return begin();
        }

        if( key.time() < front().time())
        {
            keys().insert( begin(), key);
            return begin();
        }

        if( key.time() > back().time())
        {
            keys().push_back( key);
            return end() - 1;
        }

        iterator it( lower_bound( key.time()));

        if( abs( it->time() - key.time()) <= keyframe_t::time_tolerance())
        {
		    *it = key;
            return it;
        }
        else
            return keys().insert( it, key);
    }

    iterator erase( time_type time)
    {
        iterator it( lower_bound( time));

        if( it != end())
        {
            if( abs( time - it->time()) <= keyframe_t::time_tolerance())
                return keys().erase( it);
			else
			{
				if( it != begin())
				{
					--it;

					if( abs( time - it->time()) <= keyframe_t::time_tolerance())
						return keys().erase( it);
				}
			}
        }
        
        return end();
    }

    iterator erase( iterator first, iterator last)
    {
        return keys().erase( first, last);
    }

    const_iterator lower_bound( time_type time) const
    {
        return std::lower_bound( begin(), end(), time, keyframe_less<time_type>());
    }

    iterator lower_bound( time_type time)
    {
        return std::lower_bound( begin(), end(), time, keyframe_less<time_type>());
    }

    const_iterator upper_bound( time_type time) const
    {
        return std::upper_bound( begin(), end(), time, keyframe_less<time_type>());
    }

    iterator upper_bound( time_type time)
    {
        return std::upper_bound( begin(), end(), time, keyframe_less<time_type>());
    }

	bool has_keyframe_at( time_type time) const
	{
        const_iterator it( lower_bound( time));

        if( it != end())
        {
            if( abs( time - it->time()) <= keyframe_t::time_tolerance())
                return true;
        }
		
		return false;
	}
	
private:
   
    std::vector<key_type> keys_;
};

} // namespace
} // namespace

#endif
