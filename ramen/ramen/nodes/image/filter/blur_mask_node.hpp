// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_IMAGE_BLUR_MASK_NODE_HPP
#define	RAMEN_IMAGE_BLUR_MASK_NODE_HPP

#include<ramen/nodes/image/base_blur_node.hpp>

namespace ramen
{
namespace image
{

class blur_mask_node_t : public base_blur_node_t
{
public:

    static const node_metaclass_t& blur_mask_node_metaclass();
    virtual const node_metaclass_t *metaclass() const;

    blur_mask_node_t();

protected:

    blur_mask_node_t( const blur_mask_node_t& other) : base_blur_node_t( other) {}
    void operator=( const blur_mask_node_t&);

private:

    node_t *do_clone() const { return new blur_mask_node_t( *this);}

    virtual void do_create_params();

	virtual bool do_is_identity() const;
	
    virtual void do_calc_bounds( const render::context_t& context);
    virtual void do_calc_inputs_interest( const render::context_t& context);
	
    virtual void do_process( const render::context_t& context);

	Imath::V2f get_max_blur_radius() const;
};

} // namespace
} // namespace

#endif
