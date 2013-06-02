// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_IMAGE_RESIZE_NODE_HPP
#define RAMEN_IMAGE_RESIZE_NODE_HPP

#include<ramen/nodes/image_node.hpp>

namespace ramen
{
namespace image
{

class resize_node_t : public image_node_t
{
public:

    static const node_metaclass_t& resize_node_metaclass();
    virtual const node_metaclass_t *metaclass() const;

    resize_node_t();

protected:

    resize_node_t(const resize_node_t& other);
    void operator=(const resize_node_t&);

private:

    node_t *do_clone() const { return new resize_node_t( *this);}

    virtual void do_create_params();

	virtual bool do_is_identity() const;

    virtual void do_calc_format( const render::context_t& context);
    virtual void do_calc_bounds( const render::context_t& context);
    virtual void do_calc_inputs_interest( const render::context_t& context);

    virtual void do_process( const render::context_t& context);

	Imath::V2f scale_;
};

} // namespace
} // namespace

#endif
