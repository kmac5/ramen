// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_GENERATEX_NODE_HPP
#define RAMEN_GENERATEX_NODE_HPP

#include<ramen/nodes/image/xnodes/base_expr_node.hpp>

namespace ramen
{
namespace image
{

class generatex_node_t : public base_expr_node_t
{
public:

    static const node_metaclass_t& generatex_node_metaclass();
    virtual const node_metaclass_t *metaclass() const;

    generatex_node_t();
	
	virtual expressions::se_expression_t *make_expression() const;

protected:

    generatex_node_t( const generatex_node_t& other);
    void operator=( const generatex_node_t&);

	virtual const char *expr_param_name() const { return "color_expr";}

private:

    virtual node_t *do_clone() const { return new generatex_node_t( *this);}

    virtual void do_create_params();
	
    virtual void do_calc_format( const render::context_t& context);
    virtual void do_calc_bounds( const render::context_t& context);
	virtual void do_calc_defined( const render::context_t& context);
	
    virtual void do_process( const render::context_t& context);

	void do_expression( const tbb::blocked_range<int>& range, const render::context_t& context);
	void do_aa_expression( const tbb::blocked_range<int>& range, const render::context_t& context, int samples);	
};

} // namespace
} // namespace

#endif
