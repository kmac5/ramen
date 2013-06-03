// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/anim/float_key.hpp>

namespace ramen
{
namespace anim
{
namespace
{

template<class T>
T clamp( T x, T lo, T hi)
{
    if( x < lo)
        return lo;

    if( x > hi)
        return hi;

    return x;
}

} // unnamed

float_key_t::float_key_t() : keyframe_t(), v0_( 0), v1_( 0), value_( 0)
{
    auto_v0_ = auto_v1_ = tangent_smooth;
    tangent_cont_ = true;
}

float_key_t::float_key_t( time_type time, value_type value) : keyframe_t( time), value_( value), v0_( 0), v1_( 0)
{
    auto_v0_ = auto_v1_ = tangent_smooth;
    tangent_cont_ = true;
}

void float_key_t::swap( float_key_t& other)
{
	keyframe_t::swap( other);
	std::swap( value_, other.value_);
	std::swap( v0_, other.v0_);
	std::swap( v1_, other.v1_);
	std::swap( tangent_cont_, other.tangent_cont_);
	
	for( int i = 0; i < 4; ++i)
		std::swap( coeffs_[i], other.coeffs_[i]);
}

void float_key_t::swap_value( float_key_t& other)
{
	keyframe_t::swap_value( other);
	std::swap( value_, other.value_);
	std::swap( v0_, other.v0_);
	std::swap( v1_, other.v1_);
	std::swap( tangent_cont_, other.tangent_cont_);
	
	for( int i = 0; i < 4; ++i)
		std::swap( coeffs_[i], other.coeffs_[i]);
}

void float_key_t::set_v0_auto_tangent( auto_tangent_method m)
{
    auto_v0_ = m;

    if( m == tangent_smooth || m == tangent_flat)
        set_tangent_continuity( true);
    else
    {
        if( m != tangent_fixed)
            set_tangent_continuity( false);
    }
}

void float_key_t::set_v1_auto_tangent( float_key_t::auto_tangent_method m)
{
    auto_v1_ = m;

    if( m == tangent_smooth || m == tangent_flat)
        set_tangent_continuity( true);
    else
    {
        if( m != tangent_fixed)
            set_tangent_continuity( false);
    }
}

void float_key_t::set_v0_tangent( value_type slope)
{
    v0_ = clamp( slope, min_slope(), max_slope());
    set_v0_auto_tangent( tangent_fixed);

    if( tangent_cont_)
    {
        v1_ = clamp( slope, min_slope(), max_slope());
        set_v1_auto_tangent( tangent_fixed);
    }
}

void float_key_t::set_v1_tangent( value_type slope)
{
    v1_ = clamp( slope, min_slope(), max_slope());
    set_v1_auto_tangent( tangent_fixed);

    if( tangent_cont_)
    {
        v0_ = clamp( slope, min_slope(), max_slope());
        set_v0_auto_tangent( tangent_fixed);
    }
}

void float_key_t::calc_tangents( const float_key_t *prev, const float_key_t *next)
{
    if( v0_auto_tangent() == tangent_fixed && v1_auto_tangent() == tangent_fixed)
        return;

    // left
    if( !prev || ( v0_auto_tangent() == tangent_step) || ( v0_auto_tangent() == tangent_flat))
        set_v0( 0);
    else
    {
        if( v0_auto_tangent() == tangent_linear)
            set_v0( ( value() - prev->value()) / ( time() - prev->time()));
        else
        {
            if( v0_auto_tangent() == tangent_smooth)
            {
                if( !next)
                    set_v0( 0);
                else
                    set_v0( ( next->value() - prev->value()) / ( next->time() - prev->time()));
            }
        }
    }

    // right
    if( !next || ( v1_auto_tangent() == tangent_step) || ( v1_auto_tangent() == tangent_flat))
        set_v1( 0);
    else
    {
        if( v1_auto_tangent() == tangent_linear)
            set_v1( ( next->value() - value()) / ( next->time() - time()));
        else
        {
            if( v1_auto_tangent() == tangent_smooth)
            {
                if( !prev)
                    set_v1( 0);
                else
                    set_v1( ( next->value() - prev->value()) / ( next->time() - prev->time()));
            }
        }
    }
}

void float_key_t::calc_cubic_coefficients( const float_key_t& next)
{
    if( v1_auto_tangent() == tangent_step)
    {
        coeffs_[0] = 0;
        coeffs_[1] = 0;
        coeffs_[2] = 0;
        coeffs_[3] = value();
    }
    else
    {
        float time_span = next.time() - time();
        coeffs_[0] =  ( 2 * value()) - ( 2 * next.value()) + (     time_span * v1()) + ( time_span * next.v0());
        coeffs_[1] = -( 3 * value()) + ( 3 * next.value()) - ( 2 * time_span * v1()) - ( time_span * next.v0());
        coeffs_[2] = time_span * v1();
        coeffs_[3] = value();
    }
}

float_key_t::value_type float_key_t::evaluate_cubic( time_type t) const
{    
    return (((( coeffs_[0] * t) + coeffs_[1]) * t + coeffs_[2]) * t) + coeffs_[3];
}

float_key_t::value_type float_key_t::evaluate_derivative( time_type t) const
{
    return (( 3.0 * coeffs_[0] * t) + ( 2.0 * coeffs_[1]) * t) + coeffs_[2];
}

float_key_t::value_type float_key_t::max_slope() { return 21.0;}
float_key_t::value_type float_key_t::min_slope() { return -max_slope();}

void float_key_t::str( std::stringstream& s) const
{
	s << time() << "," << value() << "," << v0() << "," << v1()
		<< "," << v0_auto_tangent() << "," << v1_auto_tangent();
}

/*
void float_key_t::read( const serialization::yaml_node_t& in)
{
	set_v0_auto_tangent( tangent_linear);
	set_v1_auto_tangent( tangent_linear);
	tangent_cont_ = false;
	
	in[0] >> time_;
	in[1] >> value_;
		
	if( in.size() > 2)
	{
		in[2] >> v0_;
		in[3] >> v1_;

		set_v0_auto_tangent( tangent_fixed);
		set_v1_auto_tangent( tangent_fixed);
		tangent_cont_ = ( v0_ == v1_);

		if( in.size() > 4)
		{
			std::string s;
			in[4] >> s;
			set_v0_auto_tangent( string_to_auto_tangent( s));

			in[5] >> s;
			set_v1_auto_tangent( string_to_auto_tangent( s));
		}
	}
}

void float_key_t::write( serialization::yaml_oarchive_t& out) const
{
    out.flow();
	out.begin_seq();
        out << time() << value() << v0() << v1();
        out << auto_tangent_to_string( v0_auto_tangent());
        out << auto_tangent_to_string( v1_auto_tangent());
    out.end_seq();
}
*/

} // anim
} // ramen
