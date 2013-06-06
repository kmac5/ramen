// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_IMAGE_BASE_EXPR_NODE_HPP
#define RAMEN_IMAGE_BASE_EXPR_NODE_HPP

#include<ramen/nodes/image_node.hpp>

#include<tbb/blocked_range.h>

#include<ramen/expressions/se_expression.hpp>

namespace ramen
{
namespace image
{

class base_expr_node_t : public image_node_t
{
public:

	enum expression_context
	{
		generate_context = 0,
		color_context,
		layer_context,
		warp_context,
		transition_context,
		general_context
	};

	class image_expression_t : public expressions::se_expression_t
	{
	public:

		struct var_t : public SeExprScalarVarRef
		{
			var_t() {}
			void eval( const SeExprVarNode* node, SeVec3d& result) { result[0] = val;}
			double val;
		};

		struct vec_var_t : public SeExprVectorVarRef
		{
			vec_var_t() {}

			explicit vec_var_t( const Imath::V2f& v)
			{
				val[0] = v[0];
				val[1] = v[1];
				val[2] = 0;
			}

			explicit vec_var_t( const Imath::V3f& v)
			{
				val[0] = v[0];
				val[1] = v[1];
				val[2] = v[2];
			}

			explicit vec_var_t( const Imath::Color4f& col)
			{
				val[0] = col.r;
				val[1] = col.g;
				val[2] = col.b;
			}

			void eval( const SeExprVarNode *node, SeVec3d& result) { result = val;}
			SeVec3d val;
		};

		image_expression_t( const std::string& expr, const base_expr_node_t *n, expression_context context);

		SeExprVarRef *resolveVar( const std::string& name) const;
		SeExprFunc *resolveFunc( const std::string& name) const;

		void setup_variables( const base_expr_node_t *n, const render::context_t& rcontext);

		mutable std::map<std::string,var_t> vars;
		mutable std::map<std::string,vec_var_t> vec_vars;

		double *get_local_var_ref( const std::string& name, double *default_ref);
		SeVec3d *get_local_var_ref( const std::string& name, SeVec3d *default_ref);

	private:

		expression_context context_;
	};

    base_expr_node_t();

	virtual expressions::se_expression_t *make_expression() const = 0;
	virtual Imath::V2f warp_amplitude() const { return Imath::V2f( 0, 0);}

protected:

    base_expr_node_t( const base_expr_node_t& other);
    void operator=( const base_expr_node_t&);

	const int max_vector_vars() const	{ return 4;}
	const int max_color_vars() const	{ return 4;}

	void create_local_var_params();

	virtual const char *expr_param_name() const = 0;

	virtual void create_expression();

    void param_changed( param_t *p, param_t::change_reason reason);

	virtual void do_calc_hash_str( const render::context_t& context);

	std::auto_ptr<image_expression_t> expr_;
};

} // namespace
} // namespace

#endif
