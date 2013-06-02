// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_IMAGE_BASE_LAYER_NODE_HPP
#define RAMEN_IMAGE_BASE_LAYER_NODE_HPP

#include<ramen/nodes/image_node.hpp>

namespace ramen
{
namespace image
{

class RAMEN_API base_layer_node_t : public image_node_t
{
public:

    base_layer_node_t();

protected:

    base_layer_node_t( const base_layer_node_t& other);
    void operator=( const base_layer_node_t&);

    void render_input( std::size_t i, const render::context_t& context);
    void release_input_image( std::size_t i);

private:

	virtual void do_calc_format( const render::context_t& context);
	
    virtual void do_recursive_process( const render::context_t& context);
};

} // namespace
} // namespace

#endif
