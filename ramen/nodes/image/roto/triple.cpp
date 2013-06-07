// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/roto/triple.hpp>

#include<algorithm>

#include<ramen/GL/gl.hpp>

#include<ramen/serialization/yaml_node.hpp>
#include<ramen/serialization/yaml_oarchive.hpp>

namespace ramen
{
namespace roto
{

triple_t::triple_t() : flags_( 0)
{
    select( false);
    set_corner( false);
    set_broken( false);
}

triple_t::triple_t( const Imath::V2f& p) : flags_( 0)
{
    set_p0( p);
    set_p1( p);
    set_p2( p);
    select( false);
    set_corner( true);
    set_broken( true);
}

void triple_t::select( bool b) const
{ 
    if( b)
		flags_ |= selected_bit;
    else
		flags_ &= ~selected_bit;
}

void triple_t::toggle_selection() const { select( !selected());}

void triple_t::set_corner( bool b)
{
    if( b)
		flags_ |= corner_bit;
    else
		flags_ &= ~corner_bit;
}

void triple_t::set_broken( bool b)
{
    if( b)
		flags_ |= broken_bit;
    else
		flags_ &= ~broken_bit;
}

void triple_t::move( const Imath::V2f& off)
{
    p0_ += off;
    p1_ += off;
    p2_ += off;
}

void triple_t::move_left_tangent( const Imath::V2f& off)    { p0_ += off;}
void triple_t::move_right_tangent( const Imath::V2f& off)   { p2_ += off;}

void triple_t::adjust_left_tangent( bool c1_cont)
{
    if( broken())
		return;

    float len = ( p0_ - p1_).length();

    if( len == 0.0f)
		return;

    Imath::V2f dir( p2_ - p1_);
    float dir_len = dir.length();

    if( dir_len == 0.0f)
		return;

    dir /= dir_len;

    if( c1_cont)
		set_p0( p1_ - ( dir * dir_len));
    else
		set_p0( p1_ - ( dir * len));
}

void triple_t::adjust_right_tangent( bool c1_cont)
{
    if( broken())
		return;
    
    float len = ( p2_ - p1_).length();
    
    if( len == 0.0f)
		return;
    
    Imath::V2f dir( p0_ - p1_);
	    float dir_len = dir.length();
    
    if( dir_len == 0.0f)
		return;
	
    dir /= dir_len;

    if( c1_cont)
		set_p2( p1_ - ( dir * dir_len));
    else
		set_p2( p1_ - ( dir * len));
}

void triple_t::convert_to_corner()
{
    set_p0( p1_);
    set_p2( p1_);
    set_corner( true);
    set_broken( true);
}

void triple_t::convert_to_curve()
{
    set_corner( false);
    set_broken( false);
}

void triple_t::read( const serialization::yaml_node_t& node, int version)
{
	switch( node.size())
	{
	case 1:
		node[0] >> p1_;
		p0_ = p1_;
		p2_ = p1_;
		set_corner( true);
		set_broken( true);
	break;
	
	case 3:
		node[0] >> p0_;
		node[1] >> p1_;
		node[2] >> p2_;
	break;
	
	case 4:
	{
		node[0] >> p0_;
		node[1] >> p1_;
		node[2] >> p2_;

		bool bk;
		node[3] >> bk;
		set_broken( bk);
	}
	break;
	
	default:
		node.error_stream() << "Error reading shape point. Unknown number of elements\n";
	};	
}

void triple_t::write( serialization::yaml_oarchive_t& out, int version) const
{
	out.begin_seq();
		if( corner())
			out << p1_;
		else
		{
			out << p0_;
			out << p1_;
			out << p2_;
			
			if( broken())
				out << true;
		}
	out.end_seq();
}

} // namespace
} // namespace
