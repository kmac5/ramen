// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_IMAGE_LENS_DISTORT_NODE_HPP
#define	RAMEN_IMAGE_LENS_DISTORT_NODE_HPP

#include<ramen/nodes/image_node.hpp>

namespace ramen
{
namespace image
{

class lens_distort_node_t : public image_node_t
{
public:

    static const node_metaclass_t& lens_distort_node_metaclass();
    virtual const node_metaclass_t *metaclass() const;

    lens_distort_node_t();

protected:

    lens_distort_node_t( const lens_distort_node_t& other);
    void operator=( const lens_distort_node_t&);

private:

    node_t *do_clone() const { return new lens_distort_node_t( *this);}

    virtual void do_create_params();
	
	virtual void do_calc_bounds( const render::context_t& context);
	virtual void do_calc_inputs_interest( const render::context_t& context);
	virtual void do_process( const render::context_t& context);

	Imath::Box2i undistort_box( const Imath::Box2i& b) const;
	Imath::Box2i redistort_box( const Imath::Box2i& b) const;
	
	template<class Fun>
	Imath::Box2i distort_box( const Imath::Box2i& b, const Imath::V2f& center, Fun dist) const
	{
		Imath::Box2f bbox;
		Imath::V2f p;
		
		p = Imath::V2f( b.min.x, b.min.y);
		p = dist( p);
		bbox.extendBy( p);
	
		p = Imath::V2f( b.max.x, b.min.y);
		p = dist( p);
		bbox.extendBy( p);
	
		p = Imath::V2f( b.max.x, b.max.y);
		p = dist( p);
		bbox.extendBy( p);
	
		p = Imath::V2f( b.min.x, b.max.y);
		p = dist( p);
		bbox.extendBy( p);
		
		if( b.min.x < center.x && b.max.x > center.x)
		{
			p = Imath::V2f( center.x, b.min.y);
			p = dist( p);
			bbox.extendBy( p);
	
			p = Imath::V2f( center.x, b.max.y);
			p = dist( p);
			bbox.extendBy( p);
		}
	
		if( b.min.y < center.y && b.max.y > center.y)
		{
			p = Imath::V2f( b.min.x, center.y);
			p = dist( p);
			bbox.extendBy( p);
	
			p = Imath::V2f( b.max.x, center.y);
			p = dist( p);
			bbox.extendBy( p);
		}
		
		return Imath::roundBox( bbox);
	}	
};

} // namespace
} // namespace

#endif
