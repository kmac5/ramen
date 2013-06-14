// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_MEMORY_MANAGER_HPP
#define RAMEN_MEMORY_MANAGER_HPP

#include<ramen/memory/manager_fwd.hpp>

#include<memory>

#include<ramen/app/application_fwd.hpp>

#include<ramen/memory/pool.hpp>
#include<ramen/memory/pool_allocator.hpp>

#include<ramen/memory/image_cache.hpp>

namespace ramen
{
namespace memory
{

class manager_t
{
public:

	typedef pool_allocator_t<pool_t>	image_allocator_type;
	typedef image_cache_t::digest_type	digest_type;

    ~manager_t();

    // caches
    void begin_interaction();
    void end_interaction();

    void clear_caches();

    void insert_in_cache( node_t *n, const digest_type& key, image::buffer_t& img);

    boost::optional<image::buffer_t> find_in_cache( const digest_type& key,
                                                    const Imath::Box2i& area);

    image_allocator_type& image_allocator() { return *img_alloc_;}

private:

    friend class ramen::application_t;

    explicit manager_t( boost::uint64_t img_cache_size);

    // non-copyable
    manager_t( const manager_t&);
    manager_t& operator=( const manager_t&);

    image_cache_t& image_cache() { return *img_cache_;}
	
    std::auto_ptr<image_allocator_type> img_alloc_;
    std::auto_ptr<image_cache_t> img_cache_;
};

} // memory
} // ramen

#endif
