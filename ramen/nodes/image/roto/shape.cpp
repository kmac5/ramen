// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/nodes/image/roto/shape.hpp>

#include<vector>

#include<boost/foreach.hpp>
#include<boost/bind.hpp>
#include<boost/range/algorithm/for_each.hpp>
#include<boost/range/algorithm/count_if.hpp>

#include<ramen/assert.hpp>

#include<ramen/algorithm/clamp.hpp>

#include<ramen/app/composition.hpp>

#include<ramen/params/bool_param.hpp>
#include<ramen/params/button_param.hpp>
#include<ramen/params/float_param.hpp>
#include<ramen/params/float2_param.hpp>

#include<ramen/anim/track.hpp>

#include<ramen/bezier/algorithm.hpp>
#include<ramen/ImathExt/ImathMatrixAlgo.h>

#include<ramen/ui/viewer/viewer.hpp>

#include<ramen/nodes/image/roto/scene.hpp>
#include<ramen/nodes/image/roto/roto_node.hpp>

#include<ramen/serialization/yaml_node.hpp>
#include<ramen/serialization/yaml_oarchive.hpp>

namespace ramen
{
namespace roto
{

shape_t::shape_t() : parameterised_t() { init();}

shape_t::shape_t( const Imath::Box2f& box) : parameterised_t()
{
    RAMEN_ASSERT( !box.isEmpty());

    init();
    triples().push_back( Imath::V2f( box.min.x, box.min.y));
    triples().push_back( Imath::V2f( box.max.x, box.min.y));
    triples().push_back( Imath::V2f( box.max.x, box.max.y));
    triples().push_back( Imath::V2f( box.min.x, box.max.y));
    close();
}

shape_t::shape_t( const shape_t& other) : parameterised_t( other), parent_( 0), scene_( 0)
{
    selected_ = false;
    display_color_ = other.display_color_;
    triples_ = other.triples_;
    closed_ = other.closed_;
    autokey_ = other.autokey_;
    track_mouse_ = other.track_mouse_;
    is_null_ = other.is_null_;
    curve_ = other.curve_;
    offset_ = other.offset_;

    xform_param_ = dynamic_cast<shape_transform_param_t*>( &param( "xform"));
    RAMEN_ASSERT( xform_param_);

    param_set().param_changed.connect( boost::bind( &shape_t::param_changed, this, _1, _2));
}

void shape_t::init()
{
    set_name( "shape");
    display_color_ = Imath::Color3c( 255, 255, 0);
    selected_ = false;
    closed_ = false;
    autokey_ = true;
    track_mouse_ = true;
    parent_ = 0;
    scene_ = 0;
    is_null_ = false;
    offset_ = Imath::V2f( 0, 0);

    param_set().param_changed.connect( boost::bind( &shape_t::param_changed, this, _1, _2));
}

shape_t *shape_t::do_clone() const { return new shape_t( *this);}

// node this parameterised belongs to
const node_t *shape_t::node() const
{
    if( scene())
        return &( scene()->parent());

    return 0;
}

node_t *shape_t::node()
{
    if( scene())
        return &( scene()->parent());

    return 0;
}

void shape_t::set_parent( shape_t *s)
{
    if( parent_)
    {
        parent_->children().erase( list_type::s_iterator_to( *this));
        parent_ = 0;
    }

    if( s)
    {
        RAMEN_ASSERT( scene() == s->scene());
        s->children().push_back( *this);
        parent_ = s;
    }

    update_xforms();
}

std::string shape_t::parent_name() const
{
    if( parent())
        return parent()->name();

    return std::string();
}

void shape_t::unparent_all_children()
{
    std::vector<shape_t*> childs;

    BOOST_FOREACH( roto::shape_t& s, children())
        childs.push_back( &s);

    boost::range::for_each( childs, boost::bind( &shape_t::set_parent, _1, (shape_t *) 0));
}

bool shape_t::find_in_children( const shape_t *shape) const
{
    if( shape == this)
        return true;

    for( list_type::const_iterator it( children().begin()), e( children().end()); it != e; ++it)
    {
        if( it->find_in_children( shape))
            return true;
    }

    return false;
}

void shape_t::do_create_params()
{
    std::auto_ptr<shape_transform_param_t> xf( new shape_transform_param_t( "Transform", "xform"));
    xform_param_ = xf.get();
    add_param( xf);

    if( !is_null())
    {
        std::auto_ptr<float2_param_t> p( new float2_param_t( "Shape Offset"));
        p->set_id( "offset");
        p->set_default_value( Imath::V2f( 0, 0));
        add_param( p);
    }

    std::auto_ptr<button_param_t> but( new button_param_t( "Apply Track..."));
    but->set_id( "apply_track");
    add_param( but);

    if( !is_null())
    {
        std::auto_ptr<bool_param_t> b( new bool_param_t( "Active"));
        b->set_id( "active");
        b->set_default_value( true);
        add_param( b);

        b.reset( new bool_param_t( "Motion blur"));
        b->set_id( "mblur");
        b->set_default_value( true);
        add_param( b);

        std::auto_ptr<float_param_t> f( new float_param_t( "Color"));
        f->set_id( "color");
        f->set_default_value( 1);
        f->set_range( 0, 1);
        f->set_step( 0.025);
        f->set_enabled( !is_null());
        add_param( f);

        f.reset( new float_param_t( "Opacity"));
        f->set_id( "opacity");
        f->set_default_value( 1);
        f->set_range( 0, 1);
        f->set_step( 0.025);
        add_param( f);

        f.reset( new float_param_t( "Grow"));
        f->set_id( "grow");
        f->set_default_value( 0);
        add_param( f);

        std::auto_ptr<float2_param_t> f2( new float2_param_t( "Blur"));
        f2->set_id( "blur");
        f2->set_min( 0);
        f2->set_default_value( Imath::V2f( 0, 0));
        f2->set_proportional( true);
        add_param( f2);

        /*
        std::auto_ptr<button_param_t> but( new button_param_t( "Apply Track to Points..."));
        but->set_id( "apply_track_pts");
        but->set_enabled( false);
        add_param( but);
        */
    }
}

void shape_t::param_changed( param_t *p, param_t::change_reason reason)
{
    if( p == &param( "apply_track"))
    {
        /*
        if( reason != param_t::user_edited)
            return;

        RAMEN_ASSERT( composition());

        image::apply_track_mode mode;
        image::apply_track_use use;

        const image::tracker_node_t *tracker = ui::apply_track_dialog_t::instance().exec( mode,  use);

        if( tracker)
        {
            xform_param_->apply_track( composition()->start_frame(), composition()->end_frame(), tracker, mode, use);
            update_xforms();
        }

        return;
        */
    }

    // bezier shapes only parameters.
    if( !is_null())
    {
        /*
        if( p == &param( "apply_track_pts"))
        {
            if( reason != param_t::user_edited)
                return;

            RAMEN_ASSERT( composition());
            // TODO: implement this

            return;
        }
        */

        if( p == &param( "offset"))
        {
            offset_ = get_value<Imath::V2f>( *p);
            return;
        }
    }
}

void shape_t::do_create_manipulators() {}

void shape_t::deselect_all_points() const
{
    boost::range::for_each( triples(), boost::bind( &triple_t::select, _1, false));
}

bool shape_t::any_point_selected() const
{
    BOOST_FOREACH( const triple_t& t, triples())
    {
        if( t.selected())
            return true;
    }

    return false;
}

bool shape_t::all_points_selected() const
{
    BOOST_FOREACH( const triple_t& t, triples())
    {
        if( !t.selected())
            return false;
    }

    return true;
}

// params
Imath::V2f shape_t::center() const
{
    return get_value<Imath::V2f>( xform_param_->center_param());
}

Imath::V2f shape_t::translation() const
{
    return get_value<Imath::V2f>( xform_param_->translate_param());
}

// edit params
void shape_t::begin_edit_params() { param_set().begin_edit();}

void shape_t::set_center( const Imath::V2f& c, bool set_key)
{
    bool autokey_state_;

    if( !set_key)
    {
        autokey_state_ = autokey();
        set_autokey( false);
    }

    xform_param_->set_center( c);
    update_xforms();

    if( !set_key)
        set_autokey( autokey_state_);
}

void shape_t::set_translation( const Imath::V2f& t, bool set_key)
{
    bool autokey_state_;

    if( !set_key)
    {
        autokey_state_ = autokey();
        set_autokey( false);
    }

    xform_param_->set_translate( t);
    update_xforms();

    if( !set_key)
        set_autokey( autokey_state_);
}

void shape_t::end_edit_params( bool notify) { param_set().end_edit( notify);}

void shape_t::update_xforms( bool motion_blur_only)
{
    if( motion_blur_only && !motion_blur())
        return;

    local_ = xform_param_->matrix();

    if( parent())
        global_ = local_ * parent()->global_xform();
    else
        global_ = local_;

    boost::range::for_each( children(), boost::bind( &shape_t::update_xforms, _1, motion_blur_only));

    inv_local_ = Imath::inverse( local_);
    inv_global_ = Imath::inverse( global_);
}

Imath::Box2f shape_t::bbox() const
{
    Imath::Box2f box;

    BOOST_FOREACH( const triple_t& t, triples())
    {
        box.extendBy( t.p0() + offset_);
        box.extendBy( t.p1() + offset_);
        box.extendBy( t.p2() + offset_);
    }

    return box;
}

Imath::Box2f shape_t::global_bbox() const
{
    Imath::Box2f box;

    BOOST_FOREACH( const triple_t& t, triples())
    {
        box.extendBy( ( t.p0() + offset_) * global_xform());
        box.extendBy( ( t.p1() + offset_) * global_xform());
        box.extendBy( ( t.p2() + offset_) * global_xform());
    }

    return box;
}

// attributes
Imath::V2f shape_t::offset() const
{
    if( is_null())
        return Imath::V2f( 0, 0);

    RAMEN_ASSERT( offset_ == get_value<Imath::V2f>( param( "offset")));

    return offset_;
}

bool shape_t::motion_blur() const
{
    if( is_null())
        return true;

    return get_value<bool>( param( "mblur"));
}

float shape_t::color() const
{
    RAMEN_ASSERT( !is_null());
    return get_value<float>( param( "color"));
}

float shape_t::opacity() const
{
    RAMEN_ASSERT( !is_null());
    return get_value<float>( param( "opacity"));
}

float shape_t::grow() const
{
    RAMEN_ASSERT( !is_null());
    return get_value<float>( param( "grow"));
}

Imath::V2f shape_t::blur() const
{
    RAMEN_ASSERT( !is_null());
    return get_value<Imath::V2f>( param( "blur"));
}

void shape_t::param_edit_finished()
{
    update_xforms();
    scene()->parent().notify();
}

bool shape_t::valid() const		{ return triples().size() >= 3;}

bool shape_t::active() const
{
    if( is_null()) // nulls are always active.
        return true;

    return get_value<bool>( param( "active"));
}

bool shape_t::is_null() const		{ return is_null_;}
void shape_t::set_is_null( bool b)
{
    RAMEN_ASSERT( param_set().empty());
    is_null_ = b;
}

bool shape_t::autokey() const		{ return autokey_;}
void shape_t::set_autokey( bool b)	{ autokey_ = b;}

bool shape_t::track_mouse() const		{ return track_mouse_;}
void shape_t::set_track_mouse( bool b)	{ track_mouse_ = b;}

void shape_t::close() { closed_ = true;}

// time & anim

void shape_t::do_set_frame( float f)
{
    evaluate_shape_curve( f);
}

// anim
class output_iterator : public std::iterator<std::output_iterator_tag, void, void, void, void>
{
public:

    explicit output_iterator( shape_t& s)
    {
        it_ = s.triples().begin();
        end_ = s.triples().end();
        index_ = 0;
    }

    output_iterator& operator=( const Imath::V2f& p)
    {
        RAMEN_ASSERT( it_ != end_);

        switch( index_)
        {
            case 0:
                it_->set_p0( p);
            break;

            case 1:
                it_->set_p1( p);
            break;

            case 2:
                it_->set_p2( p);
            break;
        }

        return *this;
    }

    output_iterator& operator*() { return *this;}

    output_iterator& operator++()
    {
        switch( index_)
        {
            case 0:
            case 1:
                ++index_;
            break;

            case 2:
                index_ = 0;
                ++it_;
            break;
        }

        return *this;
    }

    output_iterator operator++( int)
    {
        output_iterator tmp( *this);
        ++*this;
        return tmp;
    }

private:

    shape_t::triple_iterator it_;
    shape_t::triple_iterator end_;
    int index_;
};

void shape_t::evaluate_shape_curve( float f)
{
    if( !curve_.empty())
    {
        output_iterator out( *this);
        curve_.evaluate( f, out);
        // TODO: adjust tangents here!
    }
}

void shape_t::set_shape_key( float frame)
{
    anim::shape_key2f_t& key( curve_.insert( frame));

    BOOST_FOREACH( const triple_t& t, triples())
    {
        key.value().push_back( t.p0());
        key.value().push_back( t.p1());
        key.value().push_back( t.p2());
    }

    #ifndef NDEBUG
        if( curve_.size() > 1)
            RAMEN_ASSERT( key.value().size() == curve_[0].size());
    #endif
}

void shape_t::set_shape_key()
{
    if( composition())
        set_shape_key( composition()->frame());
    else
        set_shape_key( 1);
}

bool shape_t::can_delete_selected_points() const
{
    if( !any_point_selected() || all_points_selected())
        return false;

    int num_selected = boost::range::count_if( triples(),
                                               boost::bind( &triple_t::selected, _1));

    if( size() - num_selected < 3)
        return false;

    return true;
}

void shape_t::delete_selected_points()
{
    RAMEN_ASSERT( !empty());

    if( anim_curve().empty())
    {
        triples().erase( std::remove_if( triples().begin(), triples().end(),
                                         boost::bind( &triple_t::selected, _1)), triples().end());
    }
    else
    {
        while( 1)
        {
            bool any_selected = false;
            int index = 0;

            for( std::vector<triple_t>::iterator it( triples().begin()); it != triples().end(); ++it, index += 3)
            {
                if( it->selected())
                {
                    any_selected = true;
                    anim_curve().erase_points( index, 3);
                    triples().erase( it);
                    break;
                }
            }

            // post-conditions
            {
                int num_pts = anim_curve()[0].size();
                int num_triples = triples().size();
                RAMEN_ASSERT( num_pts == num_triples * 3);
            }

            if( !any_selected)
                break;
        }
    }
}

void shape_t::toggle_corner_curve_selected_points()
{
    for( triple_iterator it( triples().begin()); it != triples().end(); ++it)
    {
        if( it->selected())
        {
            if( it->corner())
            {
                it->convert_to_curve();

                triple_iterator prev, next;

                if( it == triples().begin())
                    prev = triples().end() - 1;
                else
                    prev = it - 1;

                if( it == triples().end()-1)
                    next = triples().begin();
                else
                    next = it + 1;

                Imath::V2f tn( next->p1() - prev->p1());
                tn *= 0.25f; // ????

                it->set_p0( it->p1() - tn);
                it->set_p2( it->p1() + tn);
            }
            else
                it->convert_to_corner();
        }
    }
}

void shape_t::insert_point( int span, float t)
{
    RAMEN_ASSERT( span < triples().size());

    triple_t *t0, *t1;

    if( span == triples().size() -1)
    {
        triples().push_back( triple_t());
        t0 = &triples()[span];
        t1 = &triples()[0];
    }
    else
    {
        triples().insert( triples().begin() + span + 1, triple_t());
        t0 = &triples()[span];
        t1 = t0 + 2;
    }

    triple_t *tnew = t0 + 1;

    bezier::curve_t<> c, c0, c1;
    c.p[0] = t0->p1();
    c.p[1] = t0->p2();
    c.p[2] = t1->p0();
    c.p[3] = t1->p1();

    bezier::split_curve( c, t, c0, c1);

    t0->set_p2( c0.p[1]);
    t1->set_p0( c1.p[2]);

    if( t0->corner() && t1->corner())
    {
        tnew->set_p0( c0.p[3]);
        tnew->set_p1( c0.p[3]);
        tnew->set_p2( c0.p[3]);
        tnew->set_corner( true);
    }
    else
    {
        tnew->set_p0( c0.p[2]);
        tnew->set_p1( c0.p[3]);
        tnew->set_p2( c1.p[1]);
        tnew->set_corner( false);
    }

    insert_point_in_keys( span, t);
}

void shape_t::insert_point_in_keys( int span, float t)
{
    BOOST_FOREACH( anim::shape_key2f_t& k, anim_curve().keys())
      insert_point_in_key( k, span, t);
}

void shape_t::insert_point_in_key( anim::shape_key2f_t& k, int span, float t)
{
    int num_spans = k.size() / 3;

    int i0 = span * 3 + 1;
    int i1;

    if( span == num_spans -1)
    {
        i1 = 1;
        k.append_points( 3);
    }
    else
    {
        k.insert_points( i0 + 2, 3);
        i1 = i0 + 6;
    }

    bezier::curve_t<> c, c0, c1;
    c.p[0] = k.value()[i0];
    c.p[1] = k.value()[i0 + 1];
    c.p[2] = k.value()[i1 - 1];
    c.p[3] = k.value()[i1];

    bezier::split_curve( c, t, c0, c1);

    k.value()[i0+1] = c0.p[1];

    k.value()[i0+2] = c0.p[2];
    k.value()[i0+3] = c0.p[3];
    k.value()[i0+4] = c1.p[1];

    k.value()[i1-1] = c1.p[2];
}

void shape_t::shape_curve_changed( anim::any_curve_ptr_t& c)
{
    RAMEN_ASSERT( composition());

    if( curve_.empty())
        return;

    int new_size = curve_[0].size();
    RAMEN_ASSERT( new_size % 3 == 0);
    new_size /= 3;

    if( new_size != triples_.size())
        triples_.resize( new_size);

    evaluate_shape_curve( composition()->frame());
}

void shape_t::do_create_tracks( anim::track_t *parent)
{
    if( !is_null())
    {
        std::auto_ptr<anim::track_t> t( new anim::track_t( "Points", &curve_));
        t->changed.connect( boost::bind( &shape_t::shape_curve_changed, this, _1));
        parent->add_child( t);
    }
}

// hash
void shape_t::add_to_hash_str(  hash::generator_t& hash_gen) const
{
    if( is_null() && children().empty())
        return;

    if( !active() && children().empty())
        return;

    param_set().add_to_hash( hash_gen);

    float coords[6];
    for( const_triple_iterator it( triples().begin()); it != triples().end(); ++it)
    {
        coords[0] = it->p0().x;
        coords[1] = it->p0().y;
        coords[2] = it->p1().x;
        coords[3] = it->p1().y;
        coords[4] = it->p2().x;
        coords[5] = it->p2().y;
        hash_gen.sstream().write( reinterpret_cast<char *>( coords), sizeof( float) * 6);
    }
}

bool shape_t::is_first_shape() const
{
    if( scene()->shapes().empty())
        return false;

    return &(scene()->shapes().front()) == this;
}

bool shape_t::is_last_shape() const
{
    if( scene()->shapes().empty())
        return false;

    return &(scene()->shapes().back()) == this;
}

void shape_t::read( const serialization::yaml_node_t& node, int version)
{
    std::string name;
    node.get_value( "name", name);
    set_name( name);

    // work around a bug? in yaml-cpp
    Imath::Color3f col;
    if( node.get_optional_value( "display_color", col))
    {
        display_color_.x = clamp( col.x, 0.0f, 1.0f) * 255.0f;
        display_color_.y = clamp( col.y, 0.0f, 1.0f) * 255.0f;
        display_color_.z = clamp( col.z, 0.0f, 1.0f) * 255.0f;
    }

    // parent is read by the scene...

    serialization::yaml_node_t params = node.get_node( "params");
    param_set().read( params);

    serialization::optional_yaml_node_t points = node.get_optional_node( "points");

    if( points)
    {
        RAMEN_ASSERT( !is_null());

        for( int i = 0; i < points.get().size(); ++i)
        {
            triple_t t;
            t.read( points.get()[i], version);
            triples().push_back( t);
        }

        serialization::optional_yaml_node_t curve_node( node.get_optional_node( "curve"));

        if( curve_node)
            curve_.read( curve_node.get());
    }
}

void shape_t::write( serialization::yaml_oarchive_t& out, int version) const
{
    out.begin_map();
        out << YAML::Key << "name" << YAML::Value << name();
        out << YAML::Key << "is_null" << YAML::Value << is_null();

        if( parent())
            out << YAML::Key << "parent" << YAML::Value << parent()->name();

        // work around a bug? in yaml-cpp
        Imath::Color3f col( (float) display_color().x / 255.0f,
                            (float) display_color().y / 255.0f,
                            (float) display_color().z / 255.0f);

        out << YAML::Key << "display_color" << YAML::Value << col;

        param_set().write( out);

        if( !is_null())
        {
            out << YAML::Key << "points" << YAML::Value;
                out.begin_seq();
                    boost::range::for_each( triples(), boost::bind( &triple_t::write, _1, boost::ref( out), version));
                out.end_seq();

            if( !curve_.empty())
            {
                out << YAML::Key << "curve" << YAML::Value;
                    curve_.write( out);
            }
        }

    out.end_map();
}

shape_t *new_clone( const shape_t& other)
{
    return other.clone();
}

} // roto
} // ramen
