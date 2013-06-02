// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_IMAGE_COLOR_MATCH_NODE_HPP
#define	RAMEN_IMAGE_COLOR_MATCH_NODE_HPP

#include<ramen/nodes/image_node.hpp>

namespace ramen
{
namespace image
{

class color_match_node_t : public image_node_t
{
public:

    static const node_metaclass_t& color_match_node_metaclass();
    virtual const node_metaclass_t *metaclass() const;

    color_match_node_t();

protected:

    color_match_node_t( const color_match_node_t& other) : image_node_t(other) {}
    void operator=( const color_match_node_t&);

private:

    node_t *do_clone() const { return new color_match_node_t( *this);}

    virtual void do_calc_inputs_interest( const render::context_t& context);
    virtual void do_calc_defined( const render::context_t& context);

    virtual void do_process( const render::context_t& context);
};

} // namespace
} // namespace

#endif
