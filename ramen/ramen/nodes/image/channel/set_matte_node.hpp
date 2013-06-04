// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_SET_MATTE_NODE_HPP
#define	RAMEN_SET_MATTE_NODE_HPP

#include<ramen/nodes/image_node.hpp>

namespace ramen
{
namespace image
{

class set_matte_node_t : public image_node_t
{
public:

    static const node_metaclass_t& set_matte_node_metaclass();
    virtual const node_metaclass_t *metaclass() const;

    set_matte_node_t();

    virtual bool use_cache( const render::context_t& context) const { return false;}

protected:

    set_matte_node_t( const set_matte_node_t& other) : image_node_t( other) {}
    void operator=( const set_matte_node_t&);

private:

    virtual node_t *do_clone() const { return new set_matte_node_t( *this);}

    virtual void do_create_params();

    virtual void do_calc_bounds( const render::context_t& context);

    virtual void do_process( const render::context_t& context);
};

} // namespace
} // namespace

#endif
