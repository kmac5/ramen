// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/roto/scene.hpp>

#include<cassert>
#include<map>

#include<boost/foreach.hpp>
#include<boost/range/algorithm/for_each.hpp>

#include<ramen/assert.hpp>

#include<ramen/container/ptr_vector_util.hpp>

#include<ramen/app/composition.hpp>

#include<ramen/nodes/image/roto/roto_node.hpp>

#include<ramen/serialization/yaml_node.hpp>
#include<ramen/serialization/yaml_oarchive.hpp>

namespace ramen
{
namespace roto
{

scene_t::scene_t( image::roto_node_t *p) : parent_( p) {}

scene_t::scene_t( const scene_t& other) : parent_( 0)
{
    std::map<shape_t*, const shape_t*> relation;

    BOOST_FOREACH( const shape_t& s, other.shapes())
    {
        std::auto_ptr<shape_t> new_s( new_clone( s));
        relation[ new_s.get()] = &s;

        names_.insert( new_s.get());
        new_s->set_scene( this);
        shapes_.push_back( new_s);
    }

    BOOST_FOREACH( shape_t& s, shapes())
    {
        const shape_t *old_s = relation[ &s];
        std::string parent_name = old_s->parent_name();

        if( !parent_name.empty())
        {
            shape_t *p = find_shape( parent_name);
            RAMEN_ASSERT( p);

            s.set_parent( p);
        }
    }
}

scene_t::~scene_t()
{
    // unparent all shapes before deleting the scene.
    // this avoids an assertion in boost intrusive.
    BOOST_FOREACH( shape_t& s, shapes())
    {
        s.set_parent( 0);
    }
}

void scene_t::add_shape( std::auto_ptr<shape_t> s)
{
    RAMEN_ASSERT( s->parent() == 0);

    names_.insert( s.get());
    s->set_scene( this);
    s->update_xforms();
    shapes_.push_back( s);
}

std::auto_ptr<shape_t> scene_t::release_shape( shape_t *s)
{
    std::auto_ptr<shape_t> result;
    iterator it;

    for( it = shapes_.begin(); it != shapes_.end(); ++it)
    {
        if( &*it == s)
        {
            boost::ptr_vector<shape_t>::auto_type ptr = shapes_.release( it);
            result.reset( ptr.release());
            break;
        }
    }

    if( it != shapes_.end())
    {
        s->unparent_all_children();
        s->set_parent( 0);
        names_.remove( s->name());
        result->set_scene( 0);
    }

    return result;
}

shape_t *scene_t::find_shape( const std::string& name) { return names_.find( name);}

void scene_t::rename_shape( shape_t *s, const std::string& new_name)
{
    names_.remove( s->name());
    s->set_name( new_name);
    names_.insert( s);
}

void scene_t::move_shape_order_up( shape_t *s)
{
    iterator it( iterator_for_shape( s));
    RAMEN_ASSERT( it != shapes_.end());
    RAMEN_ASSERT( it != ( shapes_.end() - 1));
    container::swap_elements( it, it+1, shapes_);
}

void scene_t::move_shape_order_down( shape_t *s)
{
    iterator it( iterator_for_shape( s));
    RAMEN_ASSERT( it != shapes_.end());
    RAMEN_ASSERT( it != shapes_.begin());
    container::swap_elements( it, it-1, shapes_);
}

scene_t::iterator scene_t::iterator_for_shape( shape_t *s)
{
    for( iterator it = shapes_.begin(); it != shapes_.end(); ++it)
    {
        if( &(*it) == s)
            return it;
    }

    return shapes_.end();
}

void scene_t::update_all_xforms( bool motion_blur_only)
{
    BOOST_FOREACH( shape_t& s, shapes())
    {
        if( !s.parent())
        {
            if( motion_blur_only && !s.motion_blur())
                continue;

            s.update_xforms( motion_blur_only);
        }
    }
}

void scene_t::set_frame( float f, bool motion_blur)
{
    BOOST_FOREACH( shape_t& s, shapes())
    {
        if( motion_blur && !s.motion_blur())
            continue;

        s.set_frame( f);
    }
}

void scene_t::add_to_hash_str( hash::generator_t& hash_gen) const
{
    boost::range::for_each( shapes(), boost::bind( &shape_t::add_to_hash_str, _1, boost::ref( hash_gen)));
}

void scene_t::add_to_hash_str( const std::vector<float>& frames, hash::generator_t& hash_gen) const
{
    float current_frame = parent().composition()->frame();

    BOOST_FOREACH( const shape_t& s, shapes())
    {
        if( !s.motion_blur())
            s.add_to_hash_str( hash_gen);
    }

    // this is not very elegant...
    scene_t *self = const_cast<scene_t*>( this);

    for( int i = 0; i < frames.size(); ++i)
    {
        self->set_frame( frames[i], true);

        BOOST_FOREACH( const shape_t& s, shapes())
        {
            if( s.motion_blur())
                s.add_to_hash_str( hash_gen);
        }
    }

    self->set_frame( current_frame);
}

Imath::Box2f scene_t::bounding_box() const
{
    Imath::Box2f bbox;

    BOOST_FOREACH( const shape_t& s, shapes())
        extend_bbox( bbox, s);

    return bbox;
}

Imath::Box2f scene_t::bounding_box( const std::vector<float>& frames) const
{
    float current_frame = parent().composition()->frame();

    Imath::Box2f bbox;

    BOOST_FOREACH( const shape_t& s, shapes())
    {
        if( !s.motion_blur())
            extend_bbox( bbox, s);
    }

    // this is not very elegant...
    scene_t *self = const_cast<scene_t*>( this);

    for( int i = 0; i < frames.size(); ++i)
    {
        self->set_frame( frames[i], true);
        self->update_all_xforms( true);

        BOOST_FOREACH( const shape_t& s, shapes())
        {
            if( s.motion_blur())
                extend_bbox( bbox, s);
        }
    }

    self->set_frame( current_frame);
    self->update_all_xforms();
    return bbox;
}

void scene_t::extend_bbox( Imath::Box2f& bbox, const shape_t& s) const
{
    if( !s.is_null() && s.active() && s.opacity() != 0.0f)
    {
        float grow = std::max( s.grow(), 0.0f);
        Imath::V2f blur = s.blur();

        Imath::Box2f b( s.global_bbox());
        b.min.x = b.min.x - grow - blur.x;
        b.min.y = b.min.y - grow - blur.y;
        b.max.x = b.max.x + grow + blur.x;
        b.max.y = b.max.y + grow + blur.y;
        bbox.extendBy( b);
    }
}

void scene_t::read( const serialization::yaml_node_t& node)
{
    int version;
    node.get_value( "roto_version", version);

    serialization::yaml_node_t shapes = node.get_node( "shapes");

    std::map<shape_t*,std::string> parent_map;

    for( int i = 0; i < shapes.size(); ++i)
    {
        std::auto_ptr<shape_t> newshape;

        bool is_null;
        shapes[i].get_value( "is_null", is_null);

        if( is_null)
            newshape = parent().create_null();
        else
            newshape = parent().create_shape();

        newshape->read( shapes[i], version);

        std::string parent_name;
        shapes[i].get_optional_value( "parent", parent_name);

        if( !parent_name.empty())
            parent_map[ newshape.get()] = parent_name;

        if( !is_null)
            newshape->close();

        add_shape( newshape);
    }

    for( std::map<shape_t*,std::string>::const_iterator it( parent_map.begin()); it != parent_map.end(); ++it)
    {
        shape_t *newparent = find_shape( it->second);
        it->first->set_parent( newparent);
    }
}

void scene_t::write( serialization::yaml_oarchive_t& out) const
{
    int version = 1;

    out << YAML::Key << "roto_version" << YAML::Value << version;
    out << YAML::Key << "shapes" << YAML::Value;
        out.begin_seq();
            boost::range::for_each( shapes(), boost::bind( &shape_t::write, _1, boost::ref( out), version));
        out.end_seq();
}

} // roto
} // ramen
