// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/params/numeric_param.hpp>

#include<ramen/nodes/image_node.hpp>

#include<ramen/app/composition.hpp>

namespace ramen
{

numeric_param_t::numeric_param_t( const std::string& name) : animated_param_t( name), type_( absolute) {}

numeric_param_t::numeric_param_t( const numeric_param_t& other) : animated_param_t( other), type_( other.type_)
{
    depends_on_port_ = other.depends_on_port_;
}

void numeric_param_t::set_round_to_int( bool r)
{
    if( r)
		set_flags( flags() | round_to_int_bit);
    else
		set_flags( flags() & ~round_to_int_bit);
}

float numeric_param_t::round( float x) const
{
    if( round_to_int())
        return (int) x;

    return x;
}

Imath::V2f numeric_param_t::round( const Imath::V2f& x) const
{
    if( round_to_int())
		return Imath::V2f( (int) x.x, (int) x.y);
	
	return x;
}

Imath::V3f numeric_param_t::round( const Imath::V3f& x) const
{
    if( round_to_int())
		return Imath::V3f( (int) x.x, (int) x.y, (int) x.z);

	return x;
}

void numeric_param_t::set_numeric_type( numeric_type_t type, int port)
{
    type_ = type;
    depends_on_port_ = port;
}

Imath::Box2i numeric_param_t::frame_area() const
{
	Imath::Box2i area;
	
    const image_node_t *n = dynamic_cast<const image_node_t*>( parameterised());
    RAMEN_ASSERT( n && "numeric relative params can only be used inside image nodes");
	
    if( depends_on_port_ == -1)
	{
		area = n->full_format();
		++area.max.x;
		++area.max.y;
		return area;
	}
	
    RAMEN_ASSERT( depends_on_port_ < n->num_inputs());
	
    if( n->input( depends_on_port_))
	{
		area =  n->input_as<image_node_t>( depends_on_port_)->full_format();
		++area.max.x;
		++area.max.y;
	}
	else
	    area = composition()->default_format().area();
	
    return area;
}

float numeric_param_t::relative_to_absolute( float x) const
{
    switch( type_)
    {
    case relative_x:
		return ( x * frame_area().size().x) + frame_area().min.x;

    case relative_y:
		return ( x * frame_area().size().y) + frame_area().min.y;

    case relative_size_x:
		return x * frame_area().size().x;

    case relative_size_y:
		return x * frame_area().size().y;

    default:
		return x;
    }
}

Imath::V2f numeric_param_t::relative_to_absolute( const Imath::V2f& x) const
{
    switch( type_)
    {
    case relative_xy:
		return Imath::V2f( ( x.x  * frame_area().size().x) + frame_area().min.x,
                            ( x.y * frame_area().size().y) + frame_area().min.y);

    case relative_size_xy:
		return Imath::V2f( x.x  * frame_area().size().x,
                            x.y * frame_area().size().y);

    default:
		return x;
    }
}

Imath::Box2f numeric_param_t::relative_to_absolute( const Imath::Box2f& x) const
{
    switch( type_)
    {
    case relative_xy:
		return Imath::Box2f( Imath::V2f( x.min.x * frame_area().size().x + frame_area().min.x, x.min.y * frame_area().size().y + frame_area().min.y),
                             Imath::V2f( x.max.x * frame_area().size().x + frame_area().min.x, x.max.y * frame_area().size().y + frame_area().min.y));

    case relative_size_xy:
		return Imath::Box2f( Imath::V2f( x.min.x * frame_area().size().x, x.min.y * frame_area().size().y),
                             Imath::V2f( x.max.x * frame_area().size().x, x.max.y * frame_area().size().y));

    default:
		return x;
    }
}

float numeric_param_t::absolute_to_relative( float x) const
{
    switch( type_)
    {
    case relative_x:
		return ( x - frame_area().min.x) / frame_area().size().x;

    case relative_y:
		return ( x - frame_area().min.y) / frame_area().size().y;

    case relative_size_x:
		return x / frame_area().size().x;

    case relative_size_y:
		return x / frame_area().size().y;

    default:
		return x;
    }
}

Imath::V2f numeric_param_t::absolute_to_relative( const Imath::V2f& x) const
{
    switch( type_)
    {
    case relative_xy:
		return Imath::V2f( ( x.x - frame_area().min.x) / frame_area().size().x, ( x.y - frame_area().min.y) / frame_area().size().y);

    case relative_size_xy:
		return Imath::V2f( x.x / frame_area().size().x, x.y / frame_area().size().y);

    default:
		return x;
    }
}

Imath::Box2f numeric_param_t::absolute_to_relative( const Imath::Box2f& x) const
{
    switch( type_)
    {
    case relative_xy:
		return Imath::Box2f( Imath::V2f( ( x.min.x - frame_area().min.x) / frame_area().size().x, ( x.min.y - frame_area().min.y) / frame_area().size().y),
                             Imath::V2f( ( x.max.x - frame_area().min.x) / frame_area().size().x, ( x.max.y - frame_area().min.y) / frame_area().size().y));

    case relative_size_xy:
		return Imath::Box2f( Imath::V2f( x.min.x / frame_area().size().x, x.min.y / frame_area().size().y),
                             Imath::V2f( x.max.x / frame_area().size().x, x.max.y / frame_area().size().y));
    default:
		return x;
    }
}

float numeric_param_t::absolute_min() const
{
    if( get_min() == -std::numeric_limits<float>::max())
		return get_min();

    switch( type_)
    {
    case relative_x:
		return ( get_min() * frame_area().size().x) + frame_area().min.x;

    case relative_y:
		return ( get_min() * frame_area().size().y) + frame_area().min.y;

    case relative_xy:
		return ( get_min() * frame_area().size().x) + frame_area().min.x;

    case relative_size_x:
		return get_min() * frame_area().size().x;

    case relative_size_y:
		return get_min() * frame_area().size().y;

    case relative_size_xy:
		return get_min() * frame_area().size().x;

    default:
		return get_min();
    }
}

float numeric_param_t::absolute_max() const
{
    if( get_max() == std::numeric_limits<float>::max())
		return get_max();

    switch( type_)
    {
    case relative_x:
		return ( get_max() * frame_area().size().x) + frame_area().min.x;

    case relative_y:
		return ( get_max() * frame_area().size().y) + frame_area().min.y;

    case relative_xy:
		return ( get_max() * frame_area().size().x) + frame_area().min.x;

    case relative_size_x:
		return get_max() * frame_area().size().x;

    case relative_size_y:
		return get_max() * frame_area().size().y;

    case relative_size_xy:
		return get_max() * frame_area().size().x;

    default:
		return get_max();
    }
}

void numeric_param_t::get_scale_and_offset( float& scale, float& offset) const
{
    switch( type_)
    {
    case relative_x:
		scale = frame_area().size().x;
		offset = frame_area().min.x;
	break;

    case relative_y:
		scale = frame_area().size().y;
		offset = frame_area().min.y;
	break;

    case relative_size_x:
		scale = frame_area().size().x;
		offset = 0;
	break;

    case relative_size_y:
		scale = frame_area().size().y;
		offset = 0;
	break;

    default:
		scale = 1.0f;
		offset = 0.0f;
    }
}

void numeric_param_t::get_scale_and_offset( Imath::V2f& scale, Imath::V2f& offset) const
{
    switch( type_)
    {
    case relative_xy:
		scale = frame_area().size();
		offset = frame_area().min;
	break;

    case relative_size_xy:
		scale = frame_area().size();
		offset = Imath::V2f( 0, 0);
	break;

    default:
		scale = Imath::V2f( 1, 1);
		offset = Imath::V2f( 0, 0);
    }
}

poly_param_value_t numeric_param_t::relative_to_absolute( const poly_param_value_t& val) const
{
	try
	{
		float v = val.cast<float>();
		v = relative_to_absolute( v);
		return poly_param_value_t( v);
	}
	catch( adobe::bad_cast& e) {}

	try
	{
		Imath::V2f v = val.cast<Imath::V2f>();
		v = relative_to_absolute( v);
		poly_param_indexable_value_t result( v);
		return adobe::poly_cast<poly_param_value_t&>( result);
	}
	catch( adobe::bad_cast& e) {}

	try
	{
		Imath::Box2f v = val.cast<Imath::Box2f>();
		v = relative_to_absolute( v);
		return poly_param_value_t( v);
	}
	catch( adobe::bad_cast& e) {}

	RAMEN_ASSERT( 0);
}

poly_param_value_t numeric_param_t::absolute_to_relative( const poly_param_value_t& val) const
{
	try
	{
		float v = val.cast<float>();
		v = absolute_to_relative( v);
		return poly_param_value_t( v);
	}
	catch( adobe::bad_cast& e) {}

	try
	{
		Imath::V2f v = val.cast<Imath::V2f>();
		v = absolute_to_relative( v);
		poly_param_indexable_value_t result( v);
		return adobe::poly_cast<poly_param_value_t&>( result);
	}
	catch( adobe::bad_cast& e) {}

	try
	{
		Imath::Box2f v = val.cast<Imath::Box2f>();
		v = absolute_to_relative( v);
		return poly_param_value_t( v);
	}
	catch( adobe::bad_cast& e) {}

	RAMEN_ASSERT( 0);
}

} // namespace
