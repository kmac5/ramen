// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_CONTAINER_LRU_CACHE_HPP
#define	RAMEN_CONTAINER_LRU_CACHE_HPP

#include<map>
#include<list>

#include<boost/optional.hpp>

namespace ramen
{

template<class Key, class Value>
class lru_cache_t
{
public:

	typedef Key		key_type;
	typedef Value	value_type;
	
    lru_cache_t() {}

    bool empty() const { return items_.empty();}

    void clear()
	{
		items_.clear();
		use_list_.clear();
	}

    void insert( const key_type& k, const value_type& v)
	{
		map_iterator it( items_.find( key));
		
		if( it != items_.end())
			touch( it);
		else
		{
			map_iterator result( items_.insert( map_type::value_type( key, v)));
			use_list_.push_front( result);
		}
	}
	
    boost::optional<value_type> find( const key_type& key)
	{
		map_iterator it( items_.find( key));
		
		if( it != items_.end())
		{
			touch( it);
			return it->second;
		}
	
		return boost::optional<value_type>();
	}

    void erase_lru()
	{
		if( !empty())
		{
			map_iterator it( use_list_.back());
			erase( it);
		}
	}

private:

    typedef std::map<Key, Value> map_type;
    typedef map_type::iterator map_iterator;

    void touch( map_iterator it)
	{
		use_list_.remove( it);
		use_list_.push_front( it);
	}
	
    void erase( map_iterator it)
	{
		use_list_.remove( it);
		items_.erase( it);	
	}

    map_type items_;
    std::list<map_iterator> use_list_;
};

} // namespace

#endif
