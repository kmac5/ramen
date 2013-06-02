// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/memory/image_disk_cache.hpp>

#include<vector>
#include<map>
#include<list>
#include<utility>

#include<boost/filesystem/operations.hpp>
#include<boost/filesystem/convenience.hpp>

#include<ramen/system/system.hpp>
#include<ramen/app/preferences.hpp>

#include<ramen/assert.hpp>

namespace ramen
{
namespace memory
{

struct image_disk_cache_t::impl : boost::noncopyable
{
    typedef std::multimap<digest_type, image_disk_cache_t::entry_ptr_t> map_type;
    typedef map_type::iterator map_iterator;

	impl( const boost::filesystem::path& cache_dir, std::size_t max_size) : cache_dir_( cache_dir)
	{
		size_ = 0;
		max_size_ = max_size;

		// ...
		std::vector<std::pair<boost::filesystem::path, std::time_t> > entries;
		get_files_in_directory( cache_dir_, entries);
		std::sort( entries.begin(), entries.end(), pair_entry_cmp());

		std::vector<std::pair<boost::filesystem::path, std::time_t> >::const_iterator it( entries.begin());
		for( ; it != entries.end(); ++it)
			add_entry( it->first);
	}

	bool empty() const { return items_.empty();}

	void clear()
	{
	    items_.clear();
	    use_list_.clear();
	}

	void begin_interaction()	{}
	void end_interaction()		{}

	void insert( const digest_type& key, image::buffer_t& img)
	{
	}

	image_disk_cache_t::entry_ptr_t find( const digest_type& key, const Imath::Box2i& area)
	{
		return image_disk_cache_t::entry_ptr_t();
	}

	void erase_lru()
	{
	    if( !empty())
	    {
			// remove_file( it->second.filename)
			// size_ -= it->second.size;

	        map_iterator it( use_list_.back());
	        erase( it);
	    }
	}

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

	// util

	struct pair_entry_cmp
	{
		bool operator()( const std::pair<boost::filesystem::path, std::time_t>& a,
						const std::pair<boost::filesystem::path, std::time_t>& b) const
		{
			return a.second > b.second;
		}
	};

	void get_files_in_directory( const boost::filesystem::path& p,
								 std::vector<std::pair<boost::filesystem::path, std::time_t> >& entries)
	{
		if( !boost::filesystem::exists( p))
	        return;

	    boost::filesystem::directory_iterator end_iter;
	    for( boost::filesystem::directory_iterator dir_itr( p); dir_itr != end_iter; ++dir_itr)
	    {
	        if( boost::filesystem::is_regular_file( dir_itr->status()))
			{
				boost::filesystem::path q( *dir_itr);

				if( q.extension() == ".exr")
				{
					std::time_t last_write = boost::filesystem::last_write_time( q);
					entries.push_back( std::make_pair( q, last_write));
				}
			}
			/*
	        else
	        {
	            if( boost::filesystem::is_directory( dir_itr->status()))
					load_plugins_in_directory( *dir_itr);
	        }
			*/
	    }
	}

	void add_entry( const boost::filesystem::path& p)
	{
	}

	map_type items_;
	std::list<map_iterator> use_list_;

	boost::filesystem::path cache_dir_;
	std::size_t size_, max_size_;
};

image_disk_cache_t::image_disk_cache_t( const boost::filesystem::path& p, std::size_t max_size_in_kb) : pimpl_( 0)
{
    // if cache dir does not exists, create it here
    //boost::filesystem::create_directories( p);

	// create file lock

	// if file locked ok
		//pimpl_ = new impl( p, max_size_in_kb);
}

image_disk_cache_t::~image_disk_cache_t()
{
	if( pimpl_)
	{
		// remove file lock
		delete pimpl_;
	}
}

bool image_disk_cache_t::empty() const
{
	if( pimpl_)
		return pimpl_->empty();

	return true;
}

void image_disk_cache_t::clear()
{
	if( pimpl_)
		pimpl_->clear();
}

void image_disk_cache_t::begin_interaction()
{
	if( pimpl_)
		pimpl_->begin_interaction();
}

void image_disk_cache_t::end_interaction()
{
	if( pimpl_)
		pimpl_->end_interaction();
}

void image_disk_cache_t::insert( const digest_type& key, image::buffer_t& img)
{
	if( pimpl_)
		pimpl_->insert( key, img);
}

image_disk_cache_t::entry_ptr_t image_disk_cache_t::find( const digest_type& key, const Imath::Box2i& area)
{
	if( pimpl_)
		pimpl_->find( key, area);

	return entry_ptr_t();
}

} // namespace
} // namespace
