// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_COLORX_NODE_HPP
#define RAMEN_COLORX_NODE_HPP

#include<ramen/nodes/image/xnodes/base_expr_node.hpp>

namespace ramen
{
namespace image
{

class colorx_node_t : public base_expr_node_t
{
public:

    static const node_metaclass_t& colorx_node_metaclass();
    virtual const node_metaclass_t *metaclass() const;

    colorx_node_t();
	
	virtual expressions::se_expression_t *make_expression() const;

protected:

    colorx_node_t( const colorx_node_t& other);
    void operator=( const colorx_node_t&);

	virtual const char *expr_param_name() const { return "color_expr";}

private:

    virtual node_t *do_clone() const { return new colorx_node_t( *this);}

    virtual void do_create_params();
		
    virtual void do_process( const render::context_t& context);

	void do_expression( const tbb::blocked_range<int>& range, const Imath::Box2i& area, const render::context_t& context);
};

} // namespace
} // namespace

#endif
