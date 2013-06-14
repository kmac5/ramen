// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_MEMORY_IMAGE_CACHE_HPP
#define	RAMEN_MEMORY_IMAGE_CACHE_HPP

#include<ramen/memory/lru_cache_interface.hpp>

#include<map>
#include<list>

#include<boost/optional.hpp>

#include<ramen/nodes/node_fwd.hpp>
#include<ramen/image/buffer.hpp>
#include<ramen/hash/generator.hpp>

namespace ramen
{
namespace memory
{

class image_cache_t : public lru_cache_interface
{
public:

    typedef hash::generator_t::digest_type digest_type;

    image_cache_t();
    virtual ~image_cache_t() {}

    virtual bool empty() const;
    virtual void clear();

    void begin_interaction();
    void end_interaction();

    void insert( node_t *n, const digest_type& key, image::buffer_t& img);

    boost::optional<image::buffer_t> find( const digest_type& key, const Imath::Box2i& area);

    virtual void erase_lru();
    virtual boost::posix_time::ptime lru_time() const;

private:

    // non-copyable
    image_cache_t( const image_cache_t&);
    image_cache_t& operator=( const image_cache_t&);

    struct entry_t
    {
        entry_t( image::buffer_t buf) : buffer( buf)
        {
            touch_time = boost::posix_time::microsec_clock::universal_time();
        }

        image::buffer_t buffer;
        boost::posix_time::ptime touch_time;
    };

    typedef std::multimap<digest_type, entry_t> map_type;
    typedef map_type::iterator map_iterator;

    void touch( map_iterator it);
    void erase( map_iterator it);

    map_type items_;
    std::list<map_iterator> use_list_;

    // interactions
    bool interacting_;
    std::map<node_t*, map_iterator> added_while_interacting_;
};

} // memory
} // ramen

#endif
