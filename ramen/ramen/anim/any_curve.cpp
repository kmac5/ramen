// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/anim/any_curve.hpp>

#include<ramen/anim/float_curve.hpp>
#include<ramen/anim/shape_curve.hpp>

namespace ramen
{
namespace anim
{
namespace
{

struct copy_visitor : public boost::static_visitor<>
{
    copy_visitor( any_curve_t& dst) : dst_( dst) {}

	void operator()( const float_curve_t *c)	{ dst_ = *c;}
	void operator()( const shape_curve2f_t *c)	{ dst_ = *c;}

private:

    any_curve_t& dst_;
};

struct copy_to_visitor : public boost::static_visitor<>
{
    copy_to_visitor( any_curve_ptr_t& dst) : dst_( dst) {}
	
	void operator()( const float_curve_t& c)
	{
		float_curve_t *curve = boost::get<float_curve_t*>( dst_);
		*curve = c;
	}

	void operator()( const shape_curve2f_t& c)
	{
		shape_curve2f_t *curve = boost::get<shape_curve2f_t*>( dst_);
		*curve = c;
	}
	
private:

    any_curve_ptr_t& dst_;
};

struct swap_visitor : public boost::static_visitor<>
{
    swap_visitor( any_curve_t& other) : other_( other) {}

	template<class Curve>
    void operator()( Curve *c)
    {
        Curve& curve = boost::get<Curve>( other_);
        c->swap( curve);
    }

private:

    any_curve_t& other_;
};

} // unnamed

void copy( const any_curve_ptr_t& src, any_curve_t& dst)
{
    copy_visitor v( dst);
    boost::apply_visitor( v, src);
}

void copy( const any_curve_t& src, any_curve_ptr_t& dst)
{
    copy_to_visitor v( dst);
    boost::apply_visitor( v, src);	
}

void swap( any_curve_ptr_t& a, any_curve_t& b)
{
    swap_visitor v( b);
    boost::apply_visitor( v, a);
}

} // anim
} // ramen
