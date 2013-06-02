// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_IMAGE_CROP_NODE_HPP
#define RAMEN_IMAGE_CROP_NODE_HPP

#include<ramen/nodes/image_node.hpp>

namespace ramen
{
namespace image
{

class crop_node_t : public image_node_t
{
public:

    static const node_metaclass_t& crop_node_metaclass();
    virtual const node_metaclass_t *metaclass() const;

    crop_node_t();

protected:

    crop_node_t(const crop_node_t& other) : image_node_t(other) { }
    void operator=(const crop_node_t&);

private:

    node_t *do_clone() const { return new crop_node_t(*this);}

    virtual void do_create_params();

    virtual void do_calc_format( const render::context_t& context);
    virtual void do_calc_bounds( const render::context_t& context);
    virtual void do_calc_inputs_interest( const render::context_t& context);

    virtual void do_process( const render::context_t& context);
};

} // namespace
} // namespace

#endif
