// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_IMAGE_LAYERX_NODE_HPP
#define RAMEN_IMAGE_LAYERX_NODE_HPP

#include<ramen/nodes/image/xnodes/base_expr_node.hpp>

namespace ramen
{
namespace image
{

class layerx_node_t : public base_expr_node_t
{
public:

    static const node_metaclass_t& layerx_node_metaclass();
    virtual const node_metaclass_t *metaclass() const;

    layerx_node_t();

	virtual expressions::se_expression_t *make_expression() const;

protected:

    layerx_node_t( const layerx_node_t& other);
    void operator=( const layerx_node_t&);

	virtual const char *expr_param_name() const { return "layer_expr";}

private:

    node_t *do_clone() const { return new layerx_node_t( *this);}

    virtual void do_create_params();
    void param_changed( param_t *p, param_t::change_reason reason);
	
	virtual void do_calc_format( const render::context_t& context);
	virtual void do_calc_bounds( const render::context_t& context);
	
	virtual void do_process( const render::context_t& context);
	
	void do_expression( const tbb::blocked_range<int>& range, const render::context_t& context);	
};

} // namespace
} // namespace

#endif
