// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_COLORDIFF_KEYER_NODE_HPP
#define RAMEN_COLORDIFF_KEYER_NODE_HPP

#include<ramen/nodes/image/keyer_node.hpp>

namespace ramen
{
namespace image
{

class color_diff_keyer_node_t : public keyer_node_t
{
public:

    static const node_metaclass_t& color_diff_keyer_node_metaclass();
    virtual const node_metaclass_t *metaclass() const;

    color_diff_keyer_node_t();

    virtual bool use_cache( const render::context_t& context) const { return false;}

protected:

    color_diff_keyer_node_t( const color_diff_keyer_node_t& other) : keyer_node_t( other) {}
    void operator=( const color_diff_keyer_node_t&);

private:

    node_t *do_clone() const { return new color_diff_keyer_node_t( *this);}

    virtual void do_create_params();

    virtual void do_process( const render::context_t& context);
};

} // namespace
} // namespace

#endif
