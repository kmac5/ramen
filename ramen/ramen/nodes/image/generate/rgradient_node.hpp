// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_IMAGE_RGRADIENT_NODE_HPP
#define RAMEN_IMAGE_RGRADIENT_NODE_HPP

#include<ramen/nodes/image/generator_node.hpp>

namespace ramen
{
namespace image
{

class rgradient_node_t : public generator_node_t
{
public:

    static const node_metaclass_t& rgradient_node_metaclass();
    virtual const node_metaclass_t *metaclass() const;

    rgradient_node_t();
	
protected:

    rgradient_node_t( const rgradient_node_t& other) : generator_node_t( other) {}
    void operator=( const rgradient_node_t&);

private:

    virtual node_t *do_clone() const { return new rgradient_node_t( *this);}

    virtual void do_create_params();
    virtual void do_create_manipulators();

	virtual void do_calc_bounds( const render::context_t& context);
	virtual void do_calc_defined( const render::context_t& context);
	
    virtual void do_process( const render::context_t& context);
};

} // namespace
} // namespace

#endif
