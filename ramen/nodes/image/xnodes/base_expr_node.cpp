// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/nodes/image/xnodes/base_expr_node.hpp>

#include<boost/bind.hpp>

#include<ramen/SeExpr/OcioSeExprFuncs.h>

#include<ramen/params/group_param.hpp>
#include<ramen/params/float3_param.hpp>
#include<ramen/params/string_param.hpp>
#include<ramen/params/color_param.hpp>

#include<ramen/app/composition.hpp>

namespace ramen
{
namespace image
{

base_expr_node_t::base_expr_node_t() : image_node_t()
{
	add_output_plug();
    param_set().param_changed.connect( boost::bind( &base_expr_node_t::param_changed, this, _1, _2));
}

base_expr_node_t::base_expr_node_t( const base_expr_node_t& other) : image_node_t( other)
{
    param_set().param_changed.connect( boost::bind( &base_expr_node_t::param_changed, this, _1, _2));
}

void base_expr_node_t::create_local_var_params()
{
	RAMEN_ASSERT( max_vector_vars() > 0 && max_vector_vars() < 10);
	RAMEN_ASSERT( max_color_vars()  > 0 && max_color_vars()  < 10);

	std::string suffix( "0");
    std::auto_ptr<group_param_t> g( new group_param_t( "Vars"));
    {
		for( int i = 0; i < max_vector_vars(); ++i)
		{
			std::auto_ptr<string_param_t> s( new string_param_t( std::string( "Name") + suffix));
			s->set_id( std::string( "vname") + suffix);
			s->set_default_value( std::string( "var") + suffix);
			g->add_param( s);
			
			std::auto_ptr<float3_param_t> p( new float3_param_t( std::string( "Value") + suffix));
			p->set_id( std::string( "vvalue") + suffix);
			p->set_default_value( Imath::V3f( 0, 0, 0));
			p->set_step( 0.005);
			g->add_param( p);

			suffix[0]++;
		}
	}
    add_param( g);

	suffix = "0";
    g.reset( new group_param_t( "Color Vars"));
    {
		for( int i = 0; i < max_color_vars(); ++i)
		{
			std::auto_ptr<string_param_t> s( new string_param_t( std::string( "CName") + suffix));
			s->set_id( std::string( "cname") + suffix);
			s->set_default_value( std::string( "cvar") + suffix);
			g->add_param( s);
			
			std::auto_ptr<color_param_t> p( new color_param_t( std::string( "CValue") + suffix));
			p->set_id( std::string( "cvalue") + suffix);
			p->set_is_rgba( false);
			p->set_default_value( Imath::Color4f( 0, 0, 0, 0));
			g->add_param( p);

			suffix[0]++;
		}
	}
    add_param( g);
}

void base_expr_node_t::param_changed( param_t *p, param_t::change_reason reason)
{
	if( reason == param_t::time_changed)
		return;

	if( p == &param( expr_param_name()))
	{
		create_expression();
		return;
	}

	if( dynamic_cast<string_param_t*>( p))
		create_expression();
}

void base_expr_node_t::do_calc_hash_str( const render::context_t& context)
{
	image_node_t::do_calc_hash_str( context);

	if( num_inputs() == 0)
		hash_generator() << context.subsample;

	if( !expr_.get())
		return;

	if( expr_->usesVar( "frame"))
		hash_generator() << context.frame;

	if( expr_->usesVar( "time"))
	{
		if( composition())
			hash_generator() << context.frame / ( float) composition()->frame_rate();
		else
			hash_generator() << context.frame / 25.0f;
	}
}

void base_expr_node_t::create_expression()
{
	expr_.reset( reinterpret_cast<image_expression_t*>( make_expression()));

	if( !expr_->isValid())
		expr_.reset();
}

base_expr_node_t::image_expression_t::image_expression_t( const std::string& expr, const base_expr_node_t *n,
														  expression_context context) : se_expression_t( expr)
{
	context_ = context;
		
	switch( context_)
	{
		case generate_context:
		case warp_context:
		{			
			// init local variables
			vars["xmin"] = var_t();
			vars["ymin"] = var_t();
			vars["xmax"] = var_t();
			vars["ymax"] = var_t();
			vars["w"] = var_t();
			vars["h"] = var_t();
			vars["x"] = var_t();
			vars["y"] = var_t();
			vars["aspect"] = var_t();
			vars["u"] = var_t();
			vars["v"] = var_t();
			vars["frame"] = var_t();
			vars["time"] = var_t();

			if( context_ == warp_context)
				vec_vars["amplitude"] = vec_var_t();
		}
		break;
		
		case color_context:
		{
			vec_vars["Cs"] = vec_var_t();
			vars["As"] = var_t();
		}
		break;
		
		case layer_context:
		{
			vec_vars["Cb"] = vec_var_t();
			vars["Ab"] = var_t();
			vec_vars["Cf"] = vec_var_t();
			vars["Af"] = var_t();
		}
		break;
	}

	// add local vars from params
	// set up variables from params
	std::string suffix = "0";
	for( int i = 0; i < n->max_vector_vars(); ++i)
	{
		std::string name = get_value<std::string>( n->param( std::string( "vname") + suffix));
		Imath::V3f value = get_value<Imath::V3f>( n->param( std::string( "vvalue") + suffix));
		vec_vars[name] = vec_var_t( value);
		suffix[0]++;
	}

	suffix = "0";
	for( int i = 0; i < n->max_color_vars(); ++i)
	{
		std::string name = get_value<std::string>( n->param( std::string( "cname") + suffix));
		Imath::Color4f value = get_value<Imath::Color4f>( n->param( std::string( "cvalue") + suffix));
		vec_vars[name] = vec_var_t( value);
		suffix[0]++;
	}
}

SeExprVarRef *base_expr_node_t::image_expression_t::resolveVar( const std::string& name) const
{
	std::map<std::string,var_t>::iterator i = vars.find( name);
	
	if( i != vars.end())
		return &i->second;

	std::map<std::string,vec_var_t>::iterator i2 = vec_vars.find( name);
	
	if( i2 != vec_vars.end())
		return &i2->second;

	return 0;
}

SeExprFunc *base_expr_node_t::image_expression_t::resolveFunc( const std::string& name) const
{
	if( name == "ocio_convert")
	{
		if( context_ != warp_context)
		{
			std::auto_ptr<SeExprFunc> f( createOcioConvertSeExprFunc());
			SeExprFunc *result = f.get();
		
			if( result)
				funcs_.push_back( f);
			
			return result;
		}
	}

	return expressions::se_expression_t::resolveFunc( name);
}

void base_expr_node_t::image_expression_t::setup_variables( const base_expr_node_t *n, const render::context_t& rcontext)
{
	if( context_ == generate_context || context_ == warp_context)
	{
		vars["xmin"].val = n->format().min.x;
		vars["ymin"].val = n->format().min.y;
		vars["xmax"].val = n->format().max.x;
		vars["ymax"].val = n->format().max.y;
		vars["w"].val = n->format().size().x + 1;
		vars["h"].val = n->format().size().y + 1;
		vars["aspect"].val = n->aspect_ratio();
		vars["frame"].val = rcontext.frame;
		
		if( n->composition())
			vars["time"].val = rcontext.frame / ( float) n->composition()->frame_rate();
		else
			vars["time"].val = rcontext.frame / 25.0f;
	}

	if( context_ == warp_context)
	{
		Imath::V2f amp = n->warp_amplitude() / rcontext.subsample;
		amp.x /= n->format().size().x + 1;
		amp.y /= n->format().size().y + 1;
		vec_vars["amplitude"].val = SeVec3d( amp.x, amp.y, 0);
	}
}

double *base_expr_node_t::image_expression_t::get_local_var_ref( const std::string& name, double *default_ref)
{
	const SeExpression::LocalVarTable& vtable = getLocalVars();
	SeExpression::LocalVarTable::const_iterator it = vtable.find( name);
		
	if( it != vtable.end())
	{
		const SeExprLocalVarRef *v = &( it->second);
		return const_cast<double*>( reinterpret_cast<const double*>( &( v->val)));
	}
	
	return default_ref;
}

SeVec3d *base_expr_node_t::image_expression_t::get_local_var_ref( const std::string& name, SeVec3d *default_ref)
{
	const SeExpression::LocalVarTable& vtable = getLocalVars();
	SeExpression::LocalVarTable::const_iterator it = vtable.find( name);
		
	if( it != vtable.end())
	{
		const SeExprLocalVarRef *v = &( it->second);
		return const_cast<SeVec3d*>( reinterpret_cast<const SeVec3d*>( &( v->val)));
	}
	
	return default_ref;
}

} // namespace
} // namespace
