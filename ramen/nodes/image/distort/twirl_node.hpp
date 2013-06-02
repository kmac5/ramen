// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_IMAGE_TWIRL_NODE_HPP
#define	RAMEN_IMAGE_TWIRL_NODE_HPP

#include<ramen/nodes/image/distort_node.hpp>

namespace ramen
{
namespace image
{

class twirl_node_t : public distort_node_t
{
public:

    static const node_metaclass_t& twirl_node_metaclass();
    virtual const node_metaclass_t *metaclass() const;

    twirl_node_t();

protected:

    twirl_node_t( const twirl_node_t& other) : distort_node_t( other) {}
    void operator=( const twirl_node_t&);

private:

    node_t *do_clone() const { return new twirl_node_t( *this);}

    virtual void do_create_params();
	virtual void do_create_manipulators();
	
	virtual bool do_is_identity() const;
	
    virtual void do_calc_bounds( const render::context_t& context);
    virtual void do_calc_inputs_interest( const render::context_t& context);
 
    virtual void do_process( const render::context_t& context);
};

} // namespace
} // namespace

#endif
