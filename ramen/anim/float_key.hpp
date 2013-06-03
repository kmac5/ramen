// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_ANIM_FLOAT_KEY_HPP
#define RAMEN_ANIM_FLOAT_KEY_HPP

#include<ramen/anim/keyframe.hpp>

#include<sstream>

#include<boost/array.hpp>

namespace ramen
{
namespace anim
{

class float_key_t : public keyframe_t
{
public:

    typedef float value_type;

    float_key_t();
    float_key_t( time_type time, value_type value);

	void swap( float_key_t& other);
	void swap_value( float_key_t& other);
	
    // value
    value_type  value() const  { return value_;}
    value_type& value()        { return value_;}

    void set_value( value_type x)    { value_ = x;}

    // tangents
    value_type v0() const        { return v0_;}
    void set_v0( value_type v)   { v0_ = v;}
    value_type v1() const        { return v1_;}
    void set_v1( value_type v)   { v1_ = v;}

    void set_v0_auto_tangent( auto_tangent_method m);
    void set_v1_auto_tangent( auto_tangent_method m);

    bool tangent_continuity() const         { return tangent_cont_;}
    void set_tangent_continuity( bool b)    { tangent_cont_ = b;}

    void calc_tangents( const float_key_t *prev, const float_key_t *next);

    void set_v0_tangent( value_type slope);
    void set_v1_tangent( value_type slope);

    // cubic
    void calc_cubic_coefficients( const float_key_t& next);
    value_type evaluate_cubic( time_type t) const;

    value_type evaluate_derivative( time_type t) const;

    const boost::array<value_type,4>& cubic_polynomial() const { return coeffs_;}

    // constants
    static value_type max_slope();
    static value_type min_slope();

	void str( std::stringstream& s) const;

private:

    value_type value_;
    value_type v0_, v1_; // tangents
    bool tangent_cont_;
    boost::array<value_type,4> coeffs_;
};

} // anim
} // ramen

#endif
