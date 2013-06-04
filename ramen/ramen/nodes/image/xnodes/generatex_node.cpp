// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/nodes/image/xnodes/generatex_node.hpp>

#include<boost/bind.hpp>

#include<tbb/parallel_for.h>

#include<ramen/app/composition.hpp>

#include<ramen/params/image_format_param.hpp>
#include<ramen/params/float_param.hpp>
#include<ramen/params/string_param.hpp>
#include<ramen/params/se_expr_param.hpp>

#include<ramen/image/color_bars.hpp>

namespace ramen
{
namespace image
{

generatex_node_t::generatex_node_t() : base_expr_node_t() { set_name( "generateX");}

generatex_node_t::generatex_node_t( const generatex_node_t& other) : base_expr_node_t( other)
{
	create_expression();
}

void generatex_node_t::do_create_params()
{
    std::auto_ptr<image_format_param_t> p( new image_format_param_t( "Format"));
    p->set_id( "format");
    add_param( p);
	
	std::auto_ptr<se_expr_param_t> s( new se_expr_param_t( "Color Expr", boost::bind( &base_expr_node_t::make_expression, this)));
	s->set_id( expr_param_name());
	s->set_default_value( "[0, 0, 0]");
	add_param( s);
	
    std::auto_ptr<float_param_t> f( new float_param_t( "AA Samples"));
    f->set_id( "aa_samples");
    f->set_range( 1, 8);
    f->set_default_value( 1);
    f->set_static( true);
    f->set_round_to_int( true);
    add_param( f);

	create_local_var_params();
	create_expression();
}

void generatex_node_t::do_calc_format( const render::context_t& context)
{
	image::format_t f( get_value<image::format_t>( param( "format")));
    Imath::Box2i area( f.area());
    --area.max.x;
    --area.max.y;
    set_format( area);
	set_aspect_ratio( f.aspect);
	set_proxy_scale( Imath::V2f( 1, 1));
}

void generatex_node_t::do_calc_bounds( const render::context_t& context)
{ 
	set_bounds( format());
}

void generatex_node_t::do_calc_defined( const render::context_t& context)
{ 
	set_defined( Imath::intersect( bounds(), interest()));
	//set_defined( interest());
}

void generatex_node_t::do_process( const render::context_t& context)
{
	if( !expr_.get() || !expr_->isValid())
	{
		image::make_color_bars( image_view());
		return;
	}
	
	int samples = get_value<float>( param( "aa_samples"));

	if( expr_->isThreadSafe()) // multi-threaded
	{
		tbb::parallel_for( tbb::blocked_range<int>( defined().min.y, defined().max.y + 1),
						   boost::bind( &generatex_node_t::do_aa_expression, this, _1, boost::cref( context), samples),
						   tbb::auto_partitioner());
	}
	else
	{
		tbb::blocked_range<int> range( defined().min.y, defined().max.y + 1);
		do_aa_expression( range, context, samples);
	}
}

void generatex_node_t::do_expression( const tbb::blocked_range<int>& range, const render::context_t& context)
{
	std::string color_expr = get_value<std::string>( param( expr_param_name()));
	image_expression_t expr( color_expr, this, generate_context);
	RAMEN_ASSERT( expr.isValid());

	expr.setup_variables( this, context);	
	image::image_view_t dst = image_view();
	
	double& xvar = expr.vars["x"].val;
	double& yvar = expr.vars["y"].val;
	double& uvar = expr.vars["u"].val;
	double& vvar = expr.vars["v"].val;
	
	double default_alpha = 1.0;
	double *avar = expr.get_local_var_ref( "Ao", &default_alpha);
	
	for( int y = range.begin(); y < range.end(); ++y)
	{
		image::image_view_t::x_iterator dst_it( dst.row_begin( y - defined().min.y));
		yvar = y;
		vvar = ( y - format().min.y) / ( double) format().size().y;

		for( int x = defined().min.x; x <= defined().max.x; ++x)
		{
			xvar = x;
			uvar = ( x - format().min.x) / ( double) format().size().x;
			SeVec3d result = expr.evaluate();
			*dst_it++ = image::pixel_t( result[0], result[1], result[2], *avar);
		}
	}
}

void generatex_node_t::do_aa_expression( const tbb::blocked_range<int>& range, const render::context_t& context, int samples)
{
	if( samples == 1)
	{
		do_expression( range, context);
		return;
	}

	std::string color_expr = get_value<std::string>( param( expr_param_name()));
	image_expression_t expr( color_expr, this, generate_context);
	RAMEN_ASSERT( expr.isValid());

	expr.setup_variables( this, context);
	image::image_view_t dst = image_view();

	double& xvar = expr.vars["x"].val;
	double& yvar = expr.vars["y"].val;
	double& uvar = expr.vars["u"].val;
	double& vvar = expr.vars["v"].val;

	double default_alpha = 1.0;
	double *avar = expr.get_local_var_ref( "Ao", &default_alpha);

	float subpixel_inc = 1.0f / ( samples + 1);
	float sum_weights = 1.0f / ( samples * samples);

	for( int y = range.begin(); y < range.end(); ++y)
	{
		image::image_view_t::x_iterator dst_it( dst.row_begin( y - defined().min.y));

		for( int x = defined().min.x; x <= defined().max.x; ++x)
		{
			image::pixel_t acc( 0, 0, 0, 0);

			for( int j = 0; j < samples; ++j)
			{
				for( int i = 0; i < samples; ++i)
				{
					xvar = x - 0.5 + ( i + 1) * subpixel_inc;
					yvar = y - 0.5 + ( j + 1) * subpixel_inc;
					uvar = ( xvar - format().min.x) / ( double) format().size().x;
					vvar = ( yvar - format().min.y) / ( double) format().size().y;
					SeVec3d result = expr.evaluate();

					boost::gil::get_color( acc, boost::gil::red_t())	+= result[0];
					boost::gil::get_color( acc, boost::gil::green_t())	+= result[1];
					boost::gil::get_color( acc, boost::gil::blue_t())	+= result[2];
					boost::gil::get_color( acc, boost::gil::alpha_t())	+= *avar;
				}
			}

			boost::gil::get_color( acc, boost::gil::red_t())	*= sum_weights;
			boost::gil::get_color( acc, boost::gil::green_t())	*= sum_weights;
			boost::gil::get_color( acc, boost::gil::blue_t())	*= sum_weights;
			boost::gil::get_color( acc, boost::gil::alpha_t())	*= sum_weights;
			*dst_it++ = acc;
		}
	}
}

expressions::se_expression_t *generatex_node_t::make_expression() const
{
	std::string color_expr = get_value<std::string>( param( expr_param_name()));
	return new image_expression_t( color_expr, this, generate_context);
}

// factory
node_t *create_generatex_node() { return new generatex_node_t();}

const node_metaclass_t *generatex_node_t::metaclass() const { return &generatex_node_metaclass();}

const node_metaclass_t& generatex_node_t::generatex_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.generatex";
        info.major_version = 1;
        info.minor_version = 0;
        info.menu = "Image";
        info.submenu = "Input";
        info.menu_item = "GenerateX";
		info.help = "Generate images using SeExpr expressions";
        info.create = &create_generatex_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( generatex_node_t::generatex_node_metaclass());

} // namespace
} // namespace
