// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/python/python.hpp>

#include<ramen/memory/manager.hpp>

#include<ramen/assert.hpp>

namespace ramen
{
namespace memory
{

manager_t::manager_t( boost::uint64_t size)
{
    img_disk_cache_.reset( new image_disk_cache_t( "/tmp/ramen/cache", 5 * 1024 * 1024));

    img_cache_.reset( new image_cache_t());

    img_alloc_.reset( new image_allocator_type( size));
	image_allocator().add_cache( img_cache_.get());
}

manager_t::~manager_t() {}

void manager_t::begin_interaction()
{
    image_cache().begin_interaction();
	image_disk_cache().begin_interaction();
}

void manager_t::end_interaction()
{
    image_cache().end_interaction();
	image_disk_cache().end_interaction();
}

void manager_t::clear_caches()
{
    image_cache().clear();
}

void manager_t::insert_in_cache( node_t *n, const adobe::md5_t::digest_t& key, image::buffer_t& img)
{
	image_cache().insert( n, key, img);
}

boost::optional<image::buffer_t> manager_t::find_in_cache( const adobe::md5_t::digest_t& key, const Imath::Box2i& area)
{
	boost::optional<image::buffer_t> result = image_cache().find( key, area);
	return result;
}

} // namespace
} // namespace
