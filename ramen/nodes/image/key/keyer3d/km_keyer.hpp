// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_KEYER3D_KMKEYER_HPP
#define RAMEN_KEYER3D_KMKEYER_HPP

#include<vector>

#include<OpenEXR/ImathBox.h>
#include<OpenEXR/ImathColor.h>

#include<ramen/hash/generator.hpp>

#include<ramen/serialization/archive_fwd.hpp>

namespace ramen
{
namespace keyer3d
{

class km_keyer_t
{
public:

    km_keyer_t( int max_clusters);

    void swap( km_keyer_t& other);

    float calc_alpha( const Imath::Color3f& color) const;

    struct cluster_t
    {
        cluster_t();
        explicit cluster_t( const Imath::Color3f& c);

        float operator()( const Imath::Color3f& color, float tol_factor = 1.0f, float soft_factor = 1.0f) const;

        bool inside_softness_region( const Imath::Color3f& color, float tol_factor = 1.0f, float soft_factor = 1.0f) const;

        void add_tolerance( const Imath::Color3f& c);
        void remove_tolerance( const Imath::Color3f& c);
        void add_softness( float amount);

        Imath::Color3f center;
        float r_tol, r_soft;
    };

    void reset();

    const std::vector<cluster_t>& clusters() const { return clusters_;}

    void add_tolerance( const Imath::Color3f& col);
    void add_tolerance( const std::vector<Imath::Color3f>& s);

    void remove_tolerance( const Imath::Color3f& col);
    void remove_tolerance( const std::vector<Imath::Color3f>& s);

    void add_softness( const Imath::Color3f& col, float amount);

    void set_tol_soft_factors( float t, float s);

    void update_bbox();

    void add_to_hash_str( hash::generator_t& gen) const;

    void read( const serialization::yaml_node_t& node);
    void write( serialization::yaml_oarchive_t& out) const;

private:

    cluster_t *find_nearest_cluster( const Imath::Color3f& col);

    void cluster_samples();

    bool sampling_;
    std::vector<Imath::Color3f> samples_;

    int max_clusters_;
    std::vector<cluster_t> clusters_;

    float tol_factor_, soft_factor_;

    Imath::Box3f bbox_;
};

} // namespace
} // namespace

#endif
