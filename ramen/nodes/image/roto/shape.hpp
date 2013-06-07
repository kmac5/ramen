// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_ROTO_SHAPE_HPP
#define RAMEN_ROTO_SHAPE_HPP

#include<ramen/config.hpp>

#include<ramen/nodes/image/roto/shape_fwd.hpp>

#include<ramen/params/parameterised.hpp>

#include<vector>
#include<iterator>

#include<boost/intrusive/list.hpp>
#include<boost/optional.hpp>

#include<OpenEXR/ImathBox.h>
#include<OpenEXR/ImathMatrix.h>
#include<OpenEXR/ImathColor.h>

#include<ramen/anim/any_curve.hpp>
#include<ramen/anim/shape_curve.hpp>

#include<ramen/bezier/curve.hpp>

#include<ramen/hash/generator.hpp>

#include<ramen/nodes/image/roto/scene_fwd.hpp>
#include<ramen/nodes/image/roto/triple.hpp>
#include<ramen/nodes/image/roto/shape_transform_param.hpp>

#include<ramen/serialization/archive_fwd.hpp>

namespace ramen
{
namespace roto
{

class RAMEN_API shape_t : public parameterised_t, public boost::intrusive::list_base_hook<>
{
public:

    shape_t();
    explicit shape_t( const Imath::Box2f& box);

    shape_t *clone() const { return do_clone();}

    // node this parameterised belongs to
    virtual const node_t *node() const;
    virtual node_t *node();

    bool valid() const;
    bool active() const;

    bool is_null() const;
    void set_is_null( bool b);

    bool closed() const { return closed_;}
    void close();

    bool selected() const	    { return selected_;}
    void select( bool b) const	{ selected_ = b;}
    void deselect_all_points() const;
    bool any_point_selected() const;
    bool all_points_selected() const;

    // intrusive
    typedef boost::intrusive::list<shape_t> list_type;

    // hierarchy
    const scene_t *scene() const	{ return scene_;}
    scene_t *scene()				{ return scene_;}
    void set_scene( scene_t *s)		{ scene_ = s;}

    const shape_t *parent() const	{ return parent_;}
    shape_t *parent()				{ return parent_;}

    void set_parent( shape_t *s);
    std::string parent_name() const;
    void unparent_all_children();
    bool find_in_children( const shape_t *shape) const;

    typedef list_type::const_iterator	const_child_iterator;
    typedef list_type::iterator			child_iterator;

    const list_type& children() const	{ return children_;}
    list_type& children()				{ return children_;}

    // transforms
    const Imath::M33f& local_xform() const	{ return local_;}
    const Imath::M33f& global_xform() const	{ return global_;}
    const boost::optional<Imath::M33f>& inv_local_xform() const	 { return inv_local_;}
    const boost::optional<Imath::M33f>& inv_global_xform() const { return inv_global_;}

    Imath::V2f center() const;
    Imath::V2f translation() const;

    // edit params
    void begin_edit_params();

    void set_center( const Imath::V2f& c, bool set_key = true);
    void set_translation( const Imath::V2f& t, bool set_key = true);

    void end_edit_params( bool notify = true);

    void update_xforms( bool motion_blur_only = false);

    // points
    bool empty() const			{ return triples_.empty();}
    std::size_t size() const	{ return triples_.size();}

    typedef std::vector<triple_t>::const_iterator	const_triple_iterator;
    typedef std::vector<triple_t>::iterator			triple_iterator;

    const std::vector<triple_t>& triples() const	{ return triples_;}
    std::vector<triple_t>& triples()				{ return triples_;}

    Imath::Box2f bbox() const;
    Imath::Box2f global_bbox() const;

    // attributes
    Imath::V2f offset() const;
    bool motion_blur() const;
    float color() const;
    float opacity() const;
    float grow() const;
    Imath::V2f blur() const;

    // generic algorithms
    template<class F>
    F for_each_span( F f) const
    {
        if( size() < 2)
            return f;

        bezier::curve_t<Imath::V2f> span;

        for( shape_t::const_triple_iterator it( triples().begin()); it != triples().end()-1; )
        {
            span.p[0] = it->p1();
            span.p[1] = it->p2();
            ++it;
            span.p[2] = it->p0();
            span.p[3] = it->p1();
            f( span);
        }

        if( closed())
        {
            span.p[0] = triples().back().p1();
            span.p[1] = triples().back().p2();
            span.p[2] = triples().front().p0();
            span.p[3] = triples().front().p1();
            f( span);
        }

        return f;
    }

    template<class Pred>
    bool for_each_span_while( Pred f) const
    {
        if( size() < 2)
            return false;

        bezier::curve_t<Imath::V2f> span;

        for( shape_t::const_triple_iterator it( triples().begin()); it != triples().end()-1; )
        {
            span.p[0] = it->p1();
            span.p[1] = it->p2();
            ++it;
            span.p[2] = it->p0();
            span.p[3] = it->p1();

            if( f( span))
                return true;
        }

        if( closed())
        {
            span.p[0] = triples().back().p1();
            span.p[1] = triples().back().p2();
            span.p[2] = triples().front().p0();
            span.p[3] = triples().front().p1();

            if( f( span))
                return true;
        }

        return false;
    }

    // parameterised
    virtual bool autokey() const;
    void set_autokey( bool b);

    virtual bool track_mouse() const;
    void set_track_mouse( bool b);

    virtual void param_edit_finished();

    // display
    const Imath::Color3c& display_color() const				{ return display_color_;}
    void set_display_color( const Imath::Color3c& c) const	{ display_color_ = c;}

    // anim & time
    const anim::shape_curve2f_t& anim_curve() const { return curve_;}
    anim::shape_curve2f_t& anim_curve()				{ return curve_;}

    void evaluate_shape_curve( float f);

    void set_shape_key();
    void set_shape_key( float frame);

    bool can_delete_selected_points() const;
    void delete_selected_points();

    void toggle_corner_curve_selected_points();

    void insert_point( int span, float t);

    // hash
    void add_to_hash_str( hash::generator_t& hash_gen) const;

    // misc
    bool is_first_shape() const;
    bool is_last_shape() const;

    // serialization
    void read( const serialization::yaml_node_t& node, int version = 1);
    void write( serialization::yaml_oarchive_t& out, int version = 1) const;

protected:

    shape_t( const shape_t& other);
    void operator=( const shape_t& other);

private:

    void init();

    virtual shape_t *do_clone() const;

    virtual void do_create_params();
    void param_changed( param_t *p, param_t::change_reason reason);

    virtual void do_create_manipulators();

    virtual void do_set_frame( float f);

    void shape_curve_changed( anim::any_curve_ptr_t& c);

    virtual void do_create_tracks( anim::track_t *parent);

    void insert_point_in_keys( int span, float t);
    void insert_point_in_key( anim::shape_key2f_t& k, int span, float t);

    // hierarchy
    scene_t *scene_;
    shape_t *parent_;
    list_type children_;

    Imath::M33f local_, global_;
    boost::optional<Imath::M33f> inv_local_, inv_global_;
    Imath::V2f offset_;

    shape_transform_param_t *xform_param_;

    // geom
    std::vector<triple_t> triples_;

    bool closed_;
    bool autokey_;
    bool track_mouse_;
    bool is_null_;
    mutable bool selected_;

    mutable Imath::Color3c display_color_;

    anim::shape_curve2f_t curve_;
};

RAMEN_API shape_t *new_clone( const shape_t& other);

} // roto
} // ramen

#endif
