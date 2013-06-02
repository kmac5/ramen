// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_IMAGE_FLIP_NODE_HPP
#define RAMEN_IMAGE_FLIP_NODE_HPP

#include<ramen/nodes/image_node.hpp>

#include<OpenEXR/ImathMatrix.h>

namespace ramen
{
namespace image
{

class flip_node_t : public image_node_t
{
public:

    static const node_metaclass_t& flip_node_metaclass();
    virtual const node_metaclass_t *metaclass() const;

    flip_node_t();

protected:

    flip_node_t(const flip_node_t& other) : image_node_t(other) {}
    void operator=(const flip_node_t&);

private:

    node_t *do_clone() const { return new flip_node_t(*this);}

    virtual void do_create_params();

    void calc_transform_matrix();

    virtual void do_calc_bounds( const render::context_t& context);
    virtual void do_calc_inputs_interest( const render::context_t& context);

    virtual void do_process( const render::context_t& context);

    Imath::M33d xform_, inv_xform_;
};

} // namespace
} // namespace

#endif
