// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/anim/float_curve.hpp>

#include<stdlib.h>

#include<limits>
#include<algorithm>
#include<sstream>
#include<cmath>

#include<boost/range/algorithm/for_each.hpp>

#include<OpenEXR/ImathFun.h>

#include<ramen/anim/util.hpp>

namespace ramen
{
namespace anim
{

float_curve_t::float_curve_t() : curve_t<float_key_t>()
{
    default_auto_tan_ = float_key_t::tangent_smooth;
    min_ = -std::numeric_limits<float>::max();
    max_ =  std::numeric_limits<float>::max();
	scale_ = 1.0f;
	offset_ = 0.0f;
}

void float_curve_t::copy( const float_curve_t& other, time_type offset)
{
    clear();

    BOOST_FOREACH( const float_key_t& k, other.keys())
    {
        float_key_t new_k = k;
        new_k.set_time( k.time() + offset);
        insert( new_k, false);
    }

    recalc_tangents_and_coefficients();
}

void float_curve_t::copy( const float_curve_t& other, time_type offset, time_type start, time_type end)
{
    clear();

    BOOST_FOREACH( const float_key_t& k, other.keys())
    {
        if( k.time() >= start && k.time() <= end)
        {
            float_key_t new_k = k;
            new_k.set_time( k.time() + offset);
            insert( new_k, false);
        }
    }

    recalc_tangents_and_coefficients();
}

void float_curve_t::swap( float_curve_t& other)
{
    using namespace std;
    curve_t<float_key_t>::swap( other);
    std::swap( min_, other.min_);
    std::swap( max_, other.max_);
    std::swap( default_auto_tan_, other.default_auto_tan_);
	// we don't swap scale and offset.
}

float_curve_t::iterator float_curve_t::insert( time_type time, value_type value, bool recalc)
{
    float_key_t k( time, value);
    iterator it( superclass::insert( k));
	
    if( it != begin())
    {
        it->set_v0_auto_tangent( (it - 1)->v1_auto_tangent());
        it->set_v1_auto_tangent( (it - 1)->v1_auto_tangent());
    }
    else
    {
        it->set_v0_auto_tangent( default_auto_tan_);
        it->set_v1_auto_tangent( default_auto_tan_);
    }

    if( recalc)
        recalc_tangents_and_coefficients( it);
	
	return it;
}

float_curve_t::iterator float_curve_t::insert( const float_key_t& k, bool recalc)
{
    iterator it( superclass::insert( k));
	
    if( recalc)
        recalc_tangents_and_coefficients( it);

    return it;
}

float_curve_t::iterator float_curve_t::insert( time_type time, bool recalc)
{
    value_type value = evaluate( time);
    return insert( time, value, recalc);
}

float_curve_t::value_type float_curve_t::evaluate( time_type time) const { return Imath::clamp( do_evaluate( time), min_, max_);}

float_curve_t::value_type float_curve_t::do_evaluate( time_type time) const
{
    if( empty())
		return 0;

    if( time < start_time())
    {
        switch( extrapolation())
        {
        case extrapolate_constant:
            return keys().front().value();

        case extrapolate_linear:
        {
            value_type dir = keys().front().v1();
            return keys().front().value() + (( time - keys().front().time()) * dir);
        }

        case extrapolate_repeat:
            return do_evaluate( repeat_time( time, start_time(), end_time()));
        }
    }

    if( time > end_time())
    {
        switch( extrapolation())
        {
        case extrapolate_constant:
            return keys().back().value();

        case extrapolate_linear:
        {
            value_type dir = keys().back().v0();
            return keys().back().value() + (( time - keys().back().time()) * dir);
        }

        case extrapolate_repeat:
            return do_evaluate( repeat_time( time, start_time(), end_time()));
        }
    }

    const_iterator it( keys().lower_bound( time));

    if( time == it->time())
        return it->value();

    --it;
    time_type t = ( time - it->time()) / ( (it+1)->time() - it->time());
    return it->evaluate_cubic( t);
}

float_curve_t::value_type float_curve_t::derive( time_type time) const
{
    if( empty())
        return 0;

    if( time < keys().front().time())
    {
        // extrapolate start
        return 0;
    }

    if( time > keys().back().time())
    {
        // extrapolate end
        return 0;
    }

    const_iterator it( keys().lower_bound( time));

    --it;
    time_type t = ( time - it->time()) / ( (it+1)->time() - it->time());
    return it->evaluate_derivative( t);
}

float_curve_t::value_type float_curve_t::integrate( time_type time1, time_type time2) const
{
    // TODO: we could do better
    value_type sum = 0.0f;

    for( time_type t = time1; t < time2; t += 1.0f)
		sum += evaluate( t);

    return sum;
}

void float_curve_t::recalc_coefficients()
{
    if( size() < 2)
        return;

    for( iterator it( keys().begin()); it != keys().end() - 1; ++it)
        it->calc_cubic_coefficients( *(it+1));
}

void float_curve_t::recalc_tangents_and_coefficients( iterator it)
{
    // not the best code I've ever written
    float_key_t *ptr[5] = { 0, 0, 0, 0, 0};

    if( (it-2) >= begin())	ptr[0] = &*(it-2);
    if( (it-1) >= begin())	ptr[1] = &*(it-1);

    ptr[2] = &*it;

    if( (it+1) < end())		ptr[3] = &*(it+1);
    if( (it+2) < end())		ptr[4] = &*(it+2);

    if( ptr[1])	ptr[1]->calc_tangents( ptr[0] , ptr[2]);
    if( ptr[2])	ptr[2]->calc_tangents( ptr[1] , ptr[3]);
    if( ptr[3])	ptr[3]->calc_tangents( ptr[2] , ptr[4]);

    if( ptr[0] && ptr[1]) ptr[0]->calc_cubic_coefficients( *ptr[1]);
    if( ptr[1] && ptr[2]) ptr[1]->calc_cubic_coefficients( *ptr[2]);
    if( ptr[2] && ptr[3]) ptr[2]->calc_cubic_coefficients( *ptr[3]);
    if( ptr[3] && ptr[4]) ptr[3]->calc_cubic_coefficients( *ptr[4]);
}

void float_curve_t::recalc_tangents_and_coefficients()
{
    adobe::for_each_position( keys(), boost::bind( &float_curve_t::recalc_tangents_and_coefficients, this, _1));
}

Imath::Box2f float_curve_t::bounds() const
{
    Imath::Box2f bounds;

    BOOST_FOREACH( const anim::float_key_t& k, keys())
        bounds.extendBy( Imath::V2f( k.time(), k.value()));

    return bounds;
}

Imath::Box2f float_curve_t::selection_bounds() const
{
    Imath::Box2f bounds;

    BOOST_FOREACH( const anim::float_key_t& k, keys())
    {
        if( k.selected())
            bounds.extendBy( Imath::V2f( k.time(), k.value()));
    }

    return bounds;
}

std::string float_curve_t::str() const
{
    std::stringstream s;
    s << extrapolation();
    boost::range::for_each( keys(), boost::bind( &float_key_t::str, _1, boost::ref( s)));
    return s.str();
}

/*
void float_curve_t::read( const serialization::yaml_node_t& in)
{
	std::string s;
	if( in.get_optional_value( "extrapolation", s))
		set_extrapolation( string_to_extrapolation_method( s));
		
	serialization::yaml_node_t keyframes( in.get_node( "keys"));
	
	for( int i = 0; i < keyframes.size(); ++i)
	{
		float_key_t k;
		k.read( keyframes[i]);
		
		// insert directly, bypassing all tangents adjustment code.
		keys().insert( k);
	}

	recalc_tangents_and_coefficients();
}

void float_curve_t::write( serialization::yaml_oarchive_t& out) const
{
	out.begin_map();
	out << YAML::Key << "extrapolation" << YAML::Value << extrapolation_method_to_string( extrapolation());

	out << YAML::Key << "keys" << YAML::Value;
	    out.begin_seq();
            boost::range::for_each( keys(), boost::bind( &float_key_t::write, _1, boost::ref( out)));
	    out.end_seq();

	out.end_map();
}
*/

} // namespace
} // namespace
