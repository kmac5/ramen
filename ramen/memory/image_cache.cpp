// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/python/python.hpp>

#include<ramen/memory/image_cache.hpp>

#include<ImathExt/ImathBoxAlgo.h>

#include<ramen/assert.hpp>

namespace ramen
{
namespace memory
{

image_cache_t::image_cache_t() : interacting_( false), disk_cache_( 0) {}

image_cache_t::image_cache_t( image_disk_cache_t *disk_cache) : interacting_( false), disk_cache_( disk_cache) {}

bool image_cache_t::empty() const { return items_.empty();}

void image_cache_t::clear()
{
    items_.clear();
    use_list_.clear();
}

void image_cache_t::begin_interaction()
{
	if( interacting_)
		end_interaction();

    interacting_ = true;
}

void image_cache_t::end_interaction()
{
    added_while_interacting_.clear();
    interacting_ = false;
}

void image_cache_t::insert( node_t *n, const digest_type& key, image::buffer_t& img)
{
    if( items_.find( key) != items_.end())
    {
        // check to see if the area this image buffer represents
        // is already included in the cache
        std::pair<map_iterator, map_iterator> range = items_.equal_range( key);
        for( map_iterator it( range.first); it != range.second; ++it)
        {
            if( ImathExt::isInside( it->second.buffer.bounds(), img.bounds()))
            {
				// in this case, just move the buffer to the front
				// of the use list
				touch( it);
				return;
            }
        }
    }

    // insert the image buffer in the cache
    if( interacting_)
    {
        std::map<node_t*, map_iterator>::iterator it = added_while_interacting_.find( n);

        if( it != added_while_interacting_.end())
            erase( it->second);
    }

    img.set_cached( true);
    map_iterator result( items_.insert( map_type::value_type( key, entry_t( img))));
    use_list_.push_front( result);

    if( interacting_)
        added_while_interacting_.insert( std::pair<node_t*, map_iterator>( n, result));

    // remove image buffers included inside this one
    std::pair<map_iterator, map_iterator> range = items_.equal_range( key);
    for( map_iterator it( range.first); it != range.second; )
    {
        if( it != result && ImathExt::isInside( img.bounds(), it->second.buffer.bounds()))
            erase( it++);
        else
            ++it;
    }
}

boost::optional<image::buffer_t> image_cache_t::find( const digest_type& key, const Imath::Box2i& area)
{
    if( items_.find( key) != items_.end())
    {
        std::pair<map_iterator, map_iterator> range = items_.equal_range( key);
		
        for( map_iterator it( range.first); it != range.second; ++it)
        {
            if( ImathExt::isInside( it->second.buffer.bounds(), area))
            {
                touch( it);
                return it->second.buffer;
            }
        }
	}
	
    return boost::optional<image::buffer_t>();
}

void image_cache_t::erase_lru()
{
    if( !empty())
    {
        map_iterator it( use_list_.back());
        erase( it);
    }
}

boost::posix_time::ptime image_cache_t::lru_time() const
{
	RAMEN_ASSERT( !empty());
	map_iterator it( use_list_.back());
	return it->second.touch_time;
}

void image_cache_t::touch( map_iterator it)
{
    use_list_.remove( it);
	it->second.touch_time = boost::posix_time::microsec_clock::universal_time();
	use_list_.push_front( it);
}

void image_cache_t::erase( map_iterator it)
{
	if( interacting_)
	{
		// remove the item from the added_while_interacting_ map
		for( std::map<node_t*, map_iterator>::iterator it2( added_while_interacting_.begin()); it2 != added_while_interacting_.end(); ++it2)
		{
			if( it == it2->second)
			{
				added_while_interacting_.erase( it2);
				break;
			}
		}
	}

	if( disk_cache_ && it->second.buffer.use_disk_cache())
		disk_cache_->insert( it->first, it->second.buffer);

	use_list_.remove( it);
	items_.erase( it);
}

} // namespace
} // namespace
