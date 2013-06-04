// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_IMAGE_GRID_NODE_HPP
#define RAMEN_IMAGE_GRID_NODE_HPP

#include<ramen/nodes/image/generator_node.hpp>

namespace ramen
{
namespace image
{

class grid_node_t : public generator_node_t
{
public:

    static const node_metaclass_t& grid_node_metaclass();
    virtual const node_metaclass_t *metaclass() const;

    grid_node_t();

protected:

    grid_node_t( const grid_node_t& other) : generator_node_t( other) {}
    void operator=( const grid_node_t&);

private:

    virtual node_t *do_clone() const { return new grid_node_t( *this);}

    virtual void do_create_params();

    virtual void do_process( const render::context_t& context);	
};

} // namespace
} // namespace

#endif
