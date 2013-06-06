// Copyright (c) 2012 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/xnodes/colorx_node.hpp>

#include<boost/noncopyable.hpp>
#include<boost/ptr_container/ptr_vector.hpp>

#include<tbb/parallel_for.h>

#include<ramen/app/composition.hpp>

#include<ramen/params/string_param.hpp>
#include<ramen/params/se_expr_param.hpp>

#include<ramen/image/color_bars.hpp>

namespace ramen
{
namespace image
{

colorx_node_t::colorx_node_t() : base_expr_node_t()
{ 
	set_name( "colorX");
    add_input_plug( "front", false, ui::palette_t::instance().color("front plug"), "Front");
}

colorx_node_t::colorx_node_t( const colorx_node_t& other) : base_expr_node_t( other)
{
	create_expression();
}

void colorx_node_t::do_create_params()
{
	std::auto_ptr<se_expr_param_t> s( new se_expr_param_t( "Color Expr", boost::bind( &base_expr_node_t::make_expression, this)));
	s->set_id( expr_param_name());
	s->set_default_value( "$Cs");
	add_param( s);
	
	create_local_var_params();
	create_expression();
}

void colorx_node_t::do_process( const render::context_t& context)
{
	if( !expr_.get() || !expr_->isValid())
	{
		image::make_color_bars( image_view());
		return;
	}
	
    Imath::Box2i area( ImathExt::intersect( input_as<image_node_t>()->defined(), defined()));

    if( area.isEmpty())
		return;
	
	if( expr_->isThreadSafe()) // multi-threaded
	{
		tbb::parallel_for( tbb::blocked_range<int>( 0, area.size().y + 1),
						   boost::bind( &colorx_node_t::do_expression, this, _1, area, boost::cref( context)), 
						   tbb::auto_partitioner());
	}
	else
	{
		tbb::blocked_range<int> range( 0, area.size().y + 1);
		do_expression( range, area, context);
	}
}

void colorx_node_t::do_expression( const tbb::blocked_range<int>& range, const Imath::Box2i& area, const render::context_t& context)
{
	std::string color_expr = get_value<std::string>( param( expr_param_name()));
	image_expression_t expr( color_expr, this, color_context);
	RAMEN_ASSERT( expr.isValid());

	expr.setup_variables( this, context);
	image::const_image_view_t src = input_as<image_node_t>()->const_subimage_view( area);
	image::image_view_t dst = subimage_view( area);
	
	SeVec3d& cvar = expr.vec_vars["Cs"].val;
	double& avar = expr.vars["As"].val;
	double *out_avar = expr.get_local_var_ref( "Ao", &avar);

	for( int y = range.begin(); y < range.end(); ++y)
	{
		image::const_image_view_t::x_iterator src_it( src.row_begin( y));
		image::image_view_t::x_iterator dst_it( dst.row_begin( y));

		for( int x = 0, xe = src.width(); x < xe; ++x)
		{
			cvar[0] = boost::gil::get_color( *src_it, boost::gil::red_t());
			cvar[1] = boost::gil::get_color( *src_it, boost::gil::green_t());
			cvar[2] = boost::gil::get_color( *src_it, boost::gil::blue_t());
			avar = boost::gil::get_color( *src_it, boost::gil::alpha_t());

			SeVec3d result = expr.evaluate();
			*dst_it++ = image::pixel_t( result[0], result[1], result[2], *out_avar);
			++src_it;
		}
	}
}

expressions::se_expression_t *colorx_node_t::make_expression() const
{
	std::string color_expr = get_value<std::string>( param( expr_param_name()));
	return new image_expression_t( color_expr, this, color_context);
}

// factory
node_t *create_colorx_node() { return new colorx_node_t();}

const node_metaclass_t *colorx_node_t::metaclass() const { return &colorx_node_metaclass();}

const node_metaclass_t& colorx_node_t::colorx_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.colorx";
        info.major_version = 1;
        info.minor_version = 0;
        info.menu = "Image";
        info.submenu = "Color";
        info.menu_item = "ColorX";
		info.help = "Color correct images using SeExpr expressions";
        info.create = &create_colorx_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( colorx_node_t::colorx_node_metaclass());

} // namespace
} // namespace
