// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_MEMORY_IMAGE_DISK_CACHE_HPP
#define	RAMEN_MEMORY_IMAGE_DISK_CACHE_HPP

#include<ramen/python/python.hpp>

#include<boost/noncopyable.hpp>
#include<boost/optional.hpp>
#include<boost/intrusive_ptr.hpp>

#include<ramen/ref_counted.hpp>

#include<ramen/filesystem/path.hpp>

#include<ramen/image/buffer.hpp>
#include<ramen/hash/generator.hpp>

namespace ramen
{
namespace memory
{

class image_disk_cache_t : boost::noncopyable
{
public:

    typedef hash::generator_t::digest_type digest_type;

    struct entry_t : public ref_counted_t {};

    typedef boost::intrusive_ptr<entry_t> entry_ptr_t;

    image_disk_cache_t( const boost::filesystem::path& p, std::size_t max_size_in_kb);
    ~image_disk_cache_t();

    bool empty() const;
    void clear();

    void begin_interaction();
    void end_interaction();

    void insert( const digest_type& key, image::buffer_t& img);
    entry_ptr_t find( const digest_type& key, const Imath::Box2i& area);

private:

    struct impl;
    impl *pimpl_;
};

} // namespace
} // namespace

#endif
