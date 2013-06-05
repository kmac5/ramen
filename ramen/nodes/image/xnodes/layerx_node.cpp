// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/xnodes/layerx_node.hpp>

#include<boost/bind.hpp>

#include<tbb/parallel_for.h>

#include<ramen/params/popup_param.hpp>
#include<ramen/params/float_param.hpp>
#include<ramen/params/string_param.hpp>
#include<ramen/params/se_expr_param.hpp>

#include<ramen/image/color_bars.hpp>

namespace ramen
{
namespace image
{
namespace
{
	
enum
{
	bbox_bg = 0,
	bbox_fg,
	bbox_union,
	bbox_intersection
};

} // namespace
	
layerx_node_t::layerx_node_t() : base_expr_node_t()
{
    set_name( "layerX");
    add_input_plug( "back", false, ui::palette_t::instance().color("back plug"), "Back");
    add_input_plug( "front", false, ui::palette_t::instance().color("front plug"), "Front");
}

layerx_node_t::layerx_node_t( const layerx_node_t& other) : base_expr_node_t( other)
{
	create_expression();
}

void layerx_node_t::do_create_params()
{
    std::auto_ptr<popup_param_t> p( new popup_param_t( "Mode"));
    p->set_id( "mode");
    p->menu_items() = boost::assign::list_of( "Background")( "Foreground");
    add_param( p);

    p.reset( new popup_param_t( "Bounding Box"));
    p->set_id( "bbox");
    p->menu_items() = boost::assign::list_of( "Background")( "Foreground")( "Union")( "Intersection");
	p->set_default_value( 2);
    add_param( p);

	std::auto_ptr<se_expr_param_t> s( new se_expr_param_t( "Layer Expr", boost::bind( &base_expr_node_t::make_expression, this)));
	s->set_id( expr_param_name());
	s->set_default_value( "$Ao = $Af + $Ab;\n$Cb + $Cf");
	add_param( s);
	
	create_local_var_params();
	create_expression();	
}

void layerx_node_t::do_calc_format( const render::context_t& context)
{
	image_node_t *in = 0;
	
    if( get_value<int>( param( "mode")))
		in = input_as<image_node_t>( 1);
    else
		in = input_as<image_node_t>( 0);

	set_format( in->format());
	set_aspect_ratio( in->aspect_ratio());
	set_proxy_scale( in->proxy_scale());
}

void layerx_node_t::do_calc_bounds( const render::context_t& context)
{
    Imath::Box2i bbox = input_as<image_node_t>( 0)->bounds();
    Imath::Box2i fbox = input_as<image_node_t>( 1)->bounds();
	Imath::Box2i result_box;

	switch( get_value<int>( param( "bbox")))
	{
		case bbox_bg:
			result_box = bbox;
		break;

		case bbox_fg:
			result_box = fbox;
		break;
		
		case bbox_union:
			result_box = bbox;
			result_box.extendBy( fbox);
		break;
		
		case bbox_intersection:
			result_box = ImathExt::intersect( fbox, bbox);
		break;
	}

    set_bounds( result_box);
}

void layerx_node_t::do_process( const render::context_t& context)
{
	if( defined().isEmpty())
		return;	
	
	if( !expr_.get() || !expr_->isValid())
	{
		image::make_color_bars( image_view());
		return;
	}

	if( expr_->isThreadSafe()) // multi-threaded
	{
		tbb::parallel_for( tbb::blocked_range<int>( defined().min.y, defined().max.y + 1),
						   boost::bind( &layerx_node_t::do_expression, this, _1, boost::cref( context)), 
						   tbb::auto_partitioner());
	}
	else
	{
		tbb::blocked_range<int> range( defined().min.y, defined().max.y + 1);
		do_expression( range, context);
	}	
}

void layerx_node_t::do_expression( const tbb::blocked_range<int>& range, const render::context_t& context)
{
	std::string layer_expr = get_value<std::string>( param( expr_param_name()));
	image_expression_t expr( layer_expr, this, layer_context);
	RAMEN_ASSERT( expr.isValid());

	expr.setup_variables( this, context);
	
	image_node_t *bg = input_as<image_node_t>( 0);
    Imath::Box2i bg_area( ImathExt::intersect( bg->defined(), defined()));
	image::const_image_view_t bg_view;
	
	if( !bg_area.isEmpty())
		bg_view = bg->const_image_view();
	
	image_node_t *fg = input_as<image_node_t>( 1);
    Imath::Box2i fg_area( ImathExt::intersect( fg->defined(), defined()));
	image::const_image_view_t fg_view;

	if( !fg_area.isEmpty())
		fg_view = fg->const_image_view();
	
	image::image_view_t dst = subimage_view( defined());
	
	SeVec3d& cbvar = expr.vec_vars["Cb"].val;
	double& abvar = expr.vars["Ab"].val;
	SeVec3d& cfvar = expr.vec_vars["Cf"].val;
	double& afvar = expr.vars["Af"].val;
	double *out_avar = expr.get_local_var_ref( "Ao", &abvar);
	
	Imath::V2i p;
	int xe = defined().max.x;
	
	for( p.y = range.begin(); p.y < range.end(); ++p.y)
	{
		image::image_view_t::x_iterator dst_it( dst.row_begin( p.y - defined().min.y));
		
		for( p.x = defined().min.x; p.x <= xe; ++p.x)
		{
			if( bg_area.intersects( p))
			{
				image::pixel_t q = bg_view( p.x - bg->defined().min.x, 
											p.y - bg->defined().min.y);

				cbvar[0] = boost::gil::get_color( q, boost::gil::red_t());
				cbvar[1] = boost::gil::get_color( q, boost::gil::green_t());
				cbvar[2] = boost::gil::get_color( q, boost::gil::blue_t());
				abvar = boost::gil::get_color( q, boost::gil::alpha_t());
			}
			else
			{
				cbvar[0] = cbvar[1] = cbvar[2] = 0.0;
				abvar = 0.0;
			}
			
			if( fg_area.intersects( p))
			{
				image::pixel_t q = fg_view( p.x - fg->defined().min.x, 
											p.y - fg->defined().min.y);

				cfvar[0] = boost::gil::get_color( q, boost::gil::red_t());
				cfvar[1] = boost::gil::get_color( q, boost::gil::green_t());
				cfvar[2] = boost::gil::get_color( q, boost::gil::blue_t());
				afvar = boost::gil::get_color( q, boost::gil::alpha_t());
			}
			else
			{
				cfvar[0] = cfvar[1] = cfvar[2] = 0.0;
				afvar = 0.0;
			}
			
			SeVec3d result = expr.evaluate();
			*dst_it++ = image::pixel_t( result[0], result[1], result[2], *out_avar);
		}
	}
}

expressions::se_expression_t *layerx_node_t::make_expression() const
{
	std::string layer_expr = get_value<std::string>( param( expr_param_name()));
	return new image_expression_t( layer_expr, this, layer_context);
}


// factory
node_t *create_layerx_node() { return new layerx_node_t();}

const node_metaclass_t *layerx_node_t::metaclass() const { return &layerx_node_metaclass();}

const node_metaclass_t& layerx_node_t::layerx_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.layerx";
        info.major_version = 1;
        info.minor_version = 0;
        info.menu = "Image";
        info.submenu = "Layer";
        info.menu_item = "LayerX";
        info.create = &create_layerx_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( layerx_node_t::layerx_node_metaclass());

} // namespace
} // namespace
