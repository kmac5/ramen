// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_ANIM_EXPRESSION_CURVE_HPP
#define	RAMEN_ANIM_EXPRESSION_CURVE_HPP

#include<OpenEXR/ImathBox.h>

#include<ramen/assert.hpp>

namespace ramen
{
namespace anim
{

class expression_curve_t
{
public:

    expression_curve_t();

    float get_min() const	{ return min_;}
    void set_min( float x)	{ min_ = x;}

    float get_max() const	{ return max_;}
    void set_max( float x)	{ max_ = x;}
	
    void set_range( float lo, float hi)
    {
        RAMEN_ASSERT( lo <= hi);
        min_ = lo;
        max_ = hi;
    }

    float scale() const				{ return scale_;}
    void set_scale( float x) const	{ scale_ = x;}
		
    float offset() const			{ return offset_;}
    void set_offset( float x) const	{ offset_ = x;}

	float relative_to_absolute( float x) const { return x * scale_ + offset_;}
	float absolute_to_relative( float x) const { return ( x - offset_) / scale_;}
	
    // eval
    float evaluate( float time) const;

private:
	
    float min_, max_;	
	mutable float offset_, scale_;
};

} // namespace
} // namespace

#endif
