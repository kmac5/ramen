// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/nodes/image/key/keyer3d/km_keyer.hpp>

#include<algorithm>

#include<boost/foreach.hpp>

#include<ramen/assert.hpp>

#include<ramen/stats/kmeans.hpp>

#include<ramen/serialization/yaml_node.hpp>
#include<ramen/serialization/yaml_oarchive.hpp>

namespace ramen
{
namespace keyer3d
{

km_keyer_t::cluster_t::cluster_t() : center( 0, 0, 0) { r_tol = r_soft = 0;}

km_keyer_t::cluster_t::cluster_t( const Imath::Color3f& c) : center( c) { r_tol = r_soft = 0;}

float km_keyer_t::cluster_t::operator()( const Imath::Color3f& c, float tol_factor, float soft_factor) const
{
    float d2 = ( c - center).length2();

    float d_tol = r_tol * tol_factor;

    if( d2 <  ( d_tol * d_tol))
        return 0.0f;

    float d_soft = ( d_tol + ( r_soft * soft_factor));

    if( d2 > ( d_soft * d_soft))
        return 1.0f;

    if( r_soft + soft_factor > 0)
        return ( std::sqrt( d2) - d_tol) / ( r_soft * soft_factor);
    else
        return 1.0f;
}

bool km_keyer_t::cluster_t::inside_softness_region( const Imath::Color3f& color, float tol_factor, float soft_factor) const
{
    return (*this)( color, tol_factor, soft_factor) != 1.0f;
}

void km_keyer_t::cluster_t::add_tolerance( const Imath::Color3f& c)
{
    float d = ( c - center).length();
    r_tol = std::max( r_tol, d);
}

void km_keyer_t::cluster_t::remove_tolerance( const Imath::Color3f& c)
{
    float d = ( c - center).length();
    r_tol = std::min( r_tol, d);
    r_soft = 0;
}

void km_keyer_t::cluster_t::add_softness( float amount) { r_soft = std::max( r_soft + amount, 0.0f);}

km_keyer_t::km_keyer_t( int max_clusters) : max_clusters_( max_clusters)
{
    tol_factor_ = 1.0f;
    soft_factor_ = 1.0f;
    sampling_ = true;
}

void km_keyer_t::swap( km_keyer_t& other)
{
    RAMEN_ASSERT( max_clusters_ == other.max_clusters_);

    samples_.swap( other.samples_);
    clusters_.swap( other.clusters_);
    std::swap( bbox_, other.bbox_);
    std::swap( sampling_, other.sampling_);
}

void km_keyer_t::reset()
{
    samples_.clear();
    clusters_.clear();
    bbox_ = Imath::Box3f();
    sampling_ = true;
}

float km_keyer_t::calc_alpha( const Imath::Color3f& color) const
{
    float a = 1.0f;

    if( bbox_.intersects( color))
    {
        BOOST_FOREACH( const cluster_t& c, clusters())
            a = std::min( a, c( color, tol_factor_, soft_factor_));
    }

    return a;
}

void km_keyer_t::add_tolerance( const Imath::Color3f& col)
{
    if( sampling_)
    {
        samples_.push_back( col);
        cluster_samples();
    }
    else
        find_nearest_cluster( col)->add_tolerance( col);

    update_bbox();
}

void km_keyer_t::add_tolerance( const std::vector<Imath::Color3f>& s)
{
    if( sampling_)
    {
        std::copy( s.begin(), s.end(), std::back_inserter( samples_));
        cluster_samples();
    }
    else
    {
        BOOST_FOREACH( const Imath::Color3f& col, s)
            find_nearest_cluster( col)->add_tolerance( col);
    }

    update_bbox();
}

void km_keyer_t::remove_tolerance( const Imath::Color3f& col)
{
    if( sampling_ && samples_.empty())
        return;

    for( int i = 0; i < clusters_.size(); ++i)
        clusters_[i].remove_tolerance( col);

    update_bbox();
    sampling_ = false;
}

void km_keyer_t::remove_tolerance( const std::vector<Imath::Color3f>& s)
{
    if( sampling_ && samples_.empty())
        return;

    BOOST_FOREACH( const Imath::Color3f& col, s)
    {
        for( int i = 0; i < clusters_.size(); ++i)
            clusters_[i].remove_tolerance( col);
    }

    update_bbox();
    sampling_ = false;
}

void km_keyer_t::add_softness( const Imath::Color3f& col, float amount)
{
    if( sampling_ && samples_.empty())
        return;

    find_nearest_cluster( col)->add_softness( amount);
    update_bbox();
    sampling_ = false;
}

km_keyer_t::cluster_t *km_keyer_t::find_nearest_cluster( const Imath::Color3f& col)
{
    RAMEN_ASSERT( !clusters_.empty());

    float min_dist = ( col - clusters_[0].center).length2();
    cluster_t *nearest = &( clusters_[0]);

    for( int i = 1; i < clusters_.size(); ++i)
    {
        float d = ( col - clusters_[i].center).length2();

        if( d < min_dist)
        {
            min_dist = d;
            nearest = &( clusters_[i]);
        }
    }

    return nearest;
}

void km_keyer_t::cluster_samples()
{
    clusters_.clear();

    stats::kmeans_t<Imath::Color3f> km( max_clusters_);
    km.cluster_samples( samples_);

    for( stats::kmeans_t<Imath::Color3f>::const_iterator it( km.begin()); it != km.end(); ++it)
    {
        clusters_.push_back( cluster_t( it->mean));
        clusters_.back().r_tol = std::sqrt( it->radius);
    }
}

void km_keyer_t::set_tol_soft_factors( float t, float s)
{
    RAMEN_ASSERT( t >= 0);
    RAMEN_ASSERT( s >= 0);

    tol_factor_ = t;
    soft_factor_ = s;
    update_bbox();
}

void km_keyer_t::update_bbox()
{
    bbox_ = Imath::Box3f();

    for( int i = 0; i < clusters().size(); ++i)
    {
        float d = ( clusters()[i].r_tol * tol_factor_) + ( clusters()[i].r_soft * soft_factor_);
        Imath::Box3f b( clusters()[i].center);
        b.min.x -= d;
        b.min.y -= d;
        b.min.z -= d;
        b.max.x += d;
        b.max.y += d;
        b.max.z += d;
        bbox_.extendBy( b);
    }
}

void km_keyer_t::add_to_hash_str( hash::generator_t& gen) const
{
    for( int i = 0; i < clusters().size(); ++i)
    {
        gen <<  clusters()[i].center.x <<
                clusters()[i].center.y <<
                clusters()[i].center.z <<
                clusters()[i].r_tol <<
                clusters()[i].r_soft;
    }
}

void km_keyer_t::read( const serialization::yaml_node_t& node)
{
    serialization::yaml_node_t cls( node.get_node( "keyer"));

    if( cls.size() == 0)
    {
        sampling_ = true;
        return;
    }

    sampling_ = false;

    for( int i = 1; i < cls.size(); i += 5)
    {
        cluster_t c;
        cls[i] >> c.center.x;
        cls[i+1] >> c.center.y;
        cls[i+2] >> c.center.z;
        cls[i+3] >> c.r_tol;
        cls[i+4] >> c.r_soft;
        clusters_.push_back( c);
    }

    update_bbox();
}

void km_keyer_t::write( serialization::yaml_oarchive_t& out) const
{
    out << YAML::Key << "keyer" << YAML::Value;
        out.begin_seq();

        if( sampling_ && samples_.empty())
        {
            out.end_seq();
            return;
        }

        for( int i = 0; i < clusters().size(); ++i)
        {
            out <<  clusters()[i].center.x <<
                    clusters()[i].center.y <<
                    clusters()[i].center.z <<
                    clusters()[i].r_tol <<
                    clusters()[i].r_soft;
        }
        out.end_seq();
}

} // namespace
} // namespace
