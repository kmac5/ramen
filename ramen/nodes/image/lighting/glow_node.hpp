// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_GLOW_NODE_HPP
#define	RAMEN_GLOW_NODE_HPP

#include<ramen/nodes/image_node.hpp>

namespace ramen
{
namespace image
{

class glow_node_t : public image_node_t
{
public:

    static const node_metaclass_t& glow_node_metaclass();
    virtual const node_metaclass_t *metaclass() const;

    glow_node_t();

protected:

    glow_node_t( const glow_node_t& other) : image_node_t( other) {}
    void operator=( const glow_node_t&);

private:

    node_t *do_clone() const { return new glow_node_t( *this);}

    virtual void do_create_params();

	virtual bool do_is_identity() const;
	
    virtual void do_calc_bounds( const render::context_t& context);
    virtual void do_calc_inputs_interest( const render::context_t& context);
	
    virtual void do_process( const render::context_t& context);

	void blur_channel( const image::const_image_view_t& src, int ch, const boost::gil::gray32f_view_t& tmp, 
						const Imath::V2f& radius, int iters, const image::image_view_t& dst);

    void get_expand_radius( int& hradius, int& vradius) const;
};

} // namespace
} // namespace

#endif
