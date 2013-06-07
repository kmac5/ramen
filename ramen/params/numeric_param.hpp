// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_NUMERIC_PARAM_HPP
#define	RAMEN_NUMERIC_PARAM_HPP

#include<ramen/params/animated_param.hpp>

#include<utility>

#include<OpenEXR/ImathBox.h>

#include<ramen/assert.hpp>

#include<ramen/nodes/node_fwd.hpp>

namespace ramen
{

class RAMEN_API numeric_param_t : public animated_param_t
{
    Q_OBJECT

public:

    enum numeric_type_t
    {
		absolute = 0,
		relative_x,
		relative_y,
		relative_xy,
		relative_size_x,
		relative_size_y,
		relative_size_xy
    };

    explicit numeric_param_t( const std::string& name);
	
    bool round_to_int() const { return flags() & round_to_int_bit;}
    void set_round_to_int( bool r);

    numeric_type_t numeric_type() const		{ return type_;}
    void set_numeric_type( numeric_type_t type, int port = -1);

    float relative_to_absolute( float x) const;
    Imath::V2f relative_to_absolute( const Imath::V2f& x) const;
    Imath::Box2f relative_to_absolute( const Imath::Box2f& x) const;

    float absolute_to_relative( float x) const;
    Imath::V2f absolute_to_relative( const Imath::V2f& x) const;
    Imath::Box2f absolute_to_relative( const Imath::Box2f& x) const;

protected:

    numeric_param_t( const numeric_param_t& other);
    void operator=( const numeric_param_t& other);

    float absolute_min() const;
    float absolute_max() const;

    float round( float x) const;
    Imath::V2f round( const Imath::V2f& x) const;
    Imath::V3f round( const Imath::V3f& x) const;

	void get_scale_and_offset( float& scale, float& offset) const;
	void get_scale_and_offset( Imath::V2f& scale, Imath::V2f& offset) const;
	
private:

	poly_param_value_t relative_to_absolute( const poly_param_value_t& val) const;
    poly_param_value_t absolute_to_relative( const poly_param_value_t& val) const;

    Imath::Box2i frame_area() const;

    numeric_type_t type_;
    int depends_on_port_;
};

template<class S>
S get_absolute_value( const param_t& p)
{
    const numeric_param_t *q = dynamic_cast<const numeric_param_t*>( &p);
    RAMEN_ASSERT( q);

    const poly_param_value_t& any( p.value());

	#ifdef NDEBUG
	    S v = any.cast<S>();
		return q->relative_to_absolute( v);
	#else
		try
		{
			S v = any.cast<S>();
			return q->relative_to_absolute( v);
		}
		catch( adobe::bad_cast& e)
		{
			RAMEN_ASSERT( 0 && "Bad cast exception in get_value");
		}
	#endif
}

template<class S>
S get_absolute_value_at_frame( const param_t& p, float frame)
{
    const numeric_param_t *q = dynamic_cast<const numeric_param_t*>( &p);
    RAMEN_ASSERT( q);

    const poly_param_value_t& any( p.value_at_frame( frame));

	#ifdef NDEBUG
		S v = any.cast<S>();
		return q->relative_to_absolute( v);
	#else
		try
		{
			S v = any.cast<S>();
			return q->relative_to_absolute( v);
		}
		catch( adobe::bad_cast& e)
		{
			RAMEN_ASSERT( 0 && "Bad cast exception in get_value");
		}
	#endif
}

} // namespace

#endif
