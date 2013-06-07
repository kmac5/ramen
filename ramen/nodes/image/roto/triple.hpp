// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_ROTO_TRIPLE_HPP
#define	RAMEN_ROTO_TRIPLE_HPP

#include<boost/cstdint.hpp>

#include<OpenEXR/ImathVec.h>

#include<ramen/serialization/archive_fwd.hpp>

namespace ramen
{
namespace roto
{

class triple_t
{
public:

    enum flag_bits
    {
		selected_bit	= 1 << 0,
        corner_bit		= 1 << 1,
        broken_bit		= 1 << 2
    };

    triple_t();
    triple_t( const Imath::V2f& p);

    const Imath::V2f& p0() const            { return p0_;}
    void set_p0( const Imath::V2f& p)       { p0_ = p;}

    const Imath::V2f& p1() const            { return p1_;}
    void set_p1( const Imath::V2f& p)       { p1_ = p;}

    const Imath::V2f& p2() const            { return p2_;}
    void set_p2( const Imath::V2f& p)       { p2_ = p;}

    void move( const Imath::V2f& off);
    void move_left_tangent( const Imath::V2f& off);
    void move_right_tangent( const Imath::V2f& off);

    bool selected() const { return flags_ & selected_bit;}
    void select( bool b) const;
    void toggle_selection() const;

    bool corner() const	{ return flags_ & corner_bit;}
    void set_corner( bool b);

    bool broken() const { return flags_ & broken_bit;}
    void set_broken( bool b);

    void adjust_left_tangent( bool c1_cont = false);
    void adjust_right_tangent( bool c1_cont = false);

    void convert_to_corner();
    void convert_to_curve();

	// serialization
	void read( const serialization::yaml_node_t& node, int version = 1);
	void write( serialization::yaml_oarchive_t& out, int version = 1) const;	
	
private:

    Imath::V2f p0_, p1_, p2_;
    mutable boost::uint32_t flags_;
};

} // namespace
} // namespace

#endif
