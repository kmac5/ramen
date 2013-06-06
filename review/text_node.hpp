// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_IMAGE_TEXT_NODE_HPP
#define RAMEN_IMAGE_TEXT_NODE_HPP

#include<ramen/nodes/image/generator_node.hpp>

namespace ramen
{
namespace image
{

class text_node_t : public generator_node_t
{
public:

    static const node_metaclass_t& text_node_metaclass();
    virtual const node_metaclass_t *metaclass() const;

    text_node_t();

protected:

    text_node_t( const text_node_t& other);
    void operator=( const text_node_t&);

private:

    virtual node_t *do_clone() const { return new text_node_t( *this);}

    virtual void do_create_params();
	
	void param_changed( param_t *p, param_t::change_reason reason);
	
	virtual void do_create_manipulators();
	
	virtual bool do_is_valid() const;
	
	//virtual void do_calc_bounds( const render::context_t& context);
	
    virtual void do_process( const render::context_t& context);	
};

} // namespace
} // namespace

#endif
