// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_ROTO_SCENE_HPP
#define RAMEN_ROTO_SCENE_HPP

#include<vector>

#include<boost/ptr_container/ptr_vector.hpp>

#include<ramen/assert.hpp>

#include<ramen/container/unique_name_map.hpp>

#include<ramen/nodes/image/roto/roto_node_fwd.hpp>

#include<ramen/nodes/image/roto/shape.hpp>

#include<ramen/serialization/archive_fwd.hpp>

namespace ramen
{
namespace roto
{

class scene_t
{
public:

    scene_t( image::roto_node_t *p = 0);
    scene_t( const scene_t& other);

    ~scene_t();

    const image::roto_node_t& parent() const	{ return *parent_;}
    image::roto_node_t& parent()				{ return *parent_;}
    void set_parent( image::roto_node_t *p)
    {
        RAMEN_ASSERT( p);
        parent_ = p;
    }

    void add_shape( std::auto_ptr<shape_t> s);
    std::auto_ptr<shape_t> release_shape( shape_t *s);

    const boost::ptr_vector<shape_t>& shapes() const	{ return shapes_;}
    boost::ptr_vector<shape_t>& shapes()				{ return shapes_;}

    typedef	boost::ptr_vector<shape_t>::const_iterator const_iterator;
    const_iterator begin() const { return shapes().begin();}
    const_iterator end() const { return shapes().end();}

    typedef	boost::ptr_vector<shape_t>::iterator iterator;
    iterator begin()	{ return shapes().begin();}
    iterator end()		{ return shapes().end();}

    shape_t *find_shape( const std::string& name);
    void rename_shape( shape_t *s, const std::string& new_name);

    void move_shape_order_up( shape_t *s);
    void move_shape_order_down( shape_t *s);

    void update_all_xforms( bool motion_blur_only = false);

    void set_frame( float f, bool motion_blur = false);

    // hash
    void add_to_hash_str( hash::generator_t& hash_gen) const;
    void add_to_hash_str( const std::vector<float>& frames, hash::generator_t& hash_gen) const;

    // bounding boxes
    Imath::Box2f bounding_box() const;
    Imath::Box2f bounding_box( const std::vector<float>& frames) const;

    // serialization
    void read( const serialization::yaml_node_t& node);
    void write( serialization::yaml_oarchive_t& out) const;

private:

    iterator iterator_for_shape( shape_t *s);

    void extend_bbox( Imath::Box2f& bbox, const shape_t& s) const;

    image::roto_node_t *parent_;

    boost::ptr_vector<shape_t> shapes_;
    unique_name_map_t<shape_t*> names_;
};

} // roto
} // ramen

#endif
