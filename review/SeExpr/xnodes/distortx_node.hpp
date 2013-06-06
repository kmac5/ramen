// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_IMAGE_DISTORTX_NODE_HPP
#define	RAMEN_IMAGE_DISTORTX_NODE_HPP

#include<ramen/nodes/image/xnodes/base_expr_node.hpp>

namespace ramen
{
namespace image
{

class distortx_node_t : public base_expr_node_t
{
public:

    static const node_metaclass_t& distortx_node_metaclass();
    virtual const node_metaclass_t *metaclass() const;
    
    distortx_node_t();

	virtual expressions::se_expression_t *make_expression() const;
	virtual Imath::V2f warp_amplitude() const;

protected:

    distortx_node_t( const distortx_node_t& other);
    void operator=( const distortx_node_t&);

	virtual const char *expr_param_name() const { return "warp_expr";}

private:

    node_t *do_clone() const { return new distortx_node_t( *this);}

    virtual void do_create_params();
	
    virtual void do_calc_bounds( const render::context_t& context);
    virtual void do_calc_inputs_interest( const render::context_t& context);
	
    virtual void do_process( const render::context_t& context);	
};

} // namespace
} // namespace

#endif
