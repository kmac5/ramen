// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_KEY_MIX_LAYER_NODE_HPP
#define RAMEN_KEY_MIX_LAYER_NODE_HPP

#include<ramen/nodes/image/base_layer_node.hpp>

namespace ramen
{
namespace image
{

class key_mix_layer_node_t : public base_layer_node_t
{
public:

    static const node_metaclass_t& key_mix_layer_node_metaclass();
    virtual const node_metaclass_t *metaclass() const;

    key_mix_layer_node_t();

protected:

    key_mix_layer_node_t( const key_mix_layer_node_t& other) : base_layer_node_t( other) { }
    void operator=(const key_mix_layer_node_t&);

private:

    node_t *do_clone() const { return new key_mix_layer_node_t(*this);}

    virtual void do_create_params();

    virtual void do_calc_bounds( const render::context_t& context);

    virtual void do_process( const render::context_t& context);
};

} // namespace
} // namespace

#endif
