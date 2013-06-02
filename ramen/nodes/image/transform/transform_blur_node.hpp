// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_IMAGE_TRANSFORM_BLUR_NODE_HPP
#define RAMEN_IMAGE_TRANSFORM_BLUR_NODE_HPP

#include<ramen/nodes/image_node.hpp>

namespace ramen
{

class transform_blur_node_t : public image_node_t
{
public:

    static const node_metaclass_t& transform_blur_node_metaclass();
    virtual const node_metaclass_t *metaclass() const;

    transform_blur_node_t();

protected:

    transform_blur_node_t( const transform_blur_node_t& other) : image_node_t( other) {}
    void operator=( const transform_blur_node_t&);

private:

    node_t *do_clone() const { return new transform_blur_node_t(*this);}

    virtual void do_create_params();

    virtual void do_calc_bounds( const render::context_t& context);

    virtual void do_calc_inputs_interest( const render::context_t& context);

    virtual void do_process( const render::context_t& context);
    virtual void do_process( const image::image_view_t& dst, const Imath::M33d& xf, int border_mode);
};

} // namespace

#endif
