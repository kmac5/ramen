// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_ANIM_FLOAT_CURVE_HPP
#define	RAMEN_ANIM_FLOAT_CURVE_HPP

#include<ramen/anim/curve_fwd.hpp>
#include<ramen/anim/curve.hpp>

#include<OpenEXR/ImathBox.h>

#include<ramen/assert.hpp>

#include<ramen/anim/float_key.hpp>

namespace ramen
{
namespace anim
{

class float_curve_t : public curve_t<float_key_t>
{
public:

	typedef curve_t<float_key_t>	superclass;
    typedef float_key_t				key_type;
    typedef key_type::time_type		time_type;
    typedef key_type::value_type	value_type;

    float_curve_t();

    void copy( const float_curve_t& other, time_type offset = 0.0f);
    void copy( const float_curve_t& other, time_type offset, time_type start, time_type end);

    void swap( float_curve_t& other);

    float_key_t::auto_tangent_method default_auto_tangents() const		{ return default_auto_tan_;}
    void set_default_auto_tangents( float_key_t::auto_tangent_method m) { default_auto_tan_ = m;}

    value_type get_min() const	{ return min_;}
    void set_min( value_type x) { min_ = x;}

    value_type get_max() const	{ return max_;}
    void set_max( value_type x) { max_ = x;}
	
    void set_range( value_type lo, value_type hi)
    {
        RAMEN_ASSERT( lo <= hi);
        min_ = lo;
        max_ = hi;
    }

    value_type scale() const			{ return scale_;}
    void set_scale( value_type x) const	{ scale_ = x;}
		
    value_type offset() const				{ return offset_;}
    void set_offset( value_type x) const	{ offset_ = x;}

	value_type relative_to_absolute( value_type x) const
	{
		return x * scale_ + offset_;
	}
	
	value_type absolute_to_relative( value_type x) const
	{
		return ( x - offset_) / scale_;
	}
	
    iterator insert( time_type time, value_type value, bool recalc = true);
    iterator insert( const float_key_t& k, bool recalc = true);
    iterator insert( time_type time, bool recalc = true);

    // tangents and coefficients
    void recalc_coefficients();

    void recalc_tangents_and_coefficients( iterator it);
    void recalc_tangents_and_coefficients();

    // eval
    value_type evaluate( time_type time) const;
    value_type derive( time_type time) const;
    value_type integrate( time_type time1, time_type time2) const;

    Imath::Box2f bounds() const;
    Imath::Box2f selection_bounds() const;

    std::string str() const;

private:

    value_type do_evaluate( time_type time) const;
	
    float_key_t::auto_tangent_method default_auto_tan_;
    value_type min_, max_;
	
	mutable value_type offset_, scale_;
};

} // anim
} // ramen

#endif
