// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/nodes/image/generate/grid_node.hpp>

#include<OpenEXR/ImathMath.h>

#include<agg2/agg_renderer_scanline.h>
#include<agg2/agg_rasterizer_scanline_aa.h>
#include<agg2/agg_scanline_u.h>
#include<agg2/agg_path_storage.h>
#include<agg2/agg_conv_stroke.h>

#include<ramen/nodes/node_factory.hpp>

#include<ramen/params/float2_param.hpp>
#include<ramen/params/float_param.hpp>
#include<ramen/params/color_param.hpp>

#include<ramen/image/agg_renderers.hpp>

namespace ramen
{
namespace image
{

grid_node_t::grid_node_t() : generator_node_t() { set_name( "grid");}

void grid_node_t::do_create_params()
{
    generator_node_t::do_create_params();

    std::auto_ptr<color_param_t> q( new color_param_t( "Bg Color"));
    q->set_id( "bgcol");
    q->set_is_rgba( true);
    q->set_default_value( Imath::Color4f( 0, 0, 0, 0));
    add_param( q);
    
    q.reset( new color_param_t( "Fg Color"));
    q->set_id( "fgcol");
    q->set_is_rgba( true);
    q->set_default_value( Imath::Color4f( 1, 1, 1, 1));
    add_param( q);

    std::auto_ptr<float2_param_t> p( new float2_param_t( "Size"));
    p->set_id( "size");
    p->set_default_value( Imath::V2f( 0.1, 0.1));
    p->set_numeric_type( numeric_param_t::relative_size_xy);
	p->set_min( 0);
	p->set_proportional( true);
    add_param( p);
	
    p.reset( new float2_param_t( "Line Width"));
    p->set_id( "linewidth");
    p->set_default_value( Imath::V2f( 0.01, 0.01));
    p->set_numeric_type( numeric_param_t::relative_size_xy);
	p->set_min( 0);
	p->set_proportional( true);
    add_param( p);

    p.reset( new float2_param_t( "Translate"));
    p->set_id( "translate");
    p->set_default_value( Imath::V2f( 0, 0));
    p->set_numeric_type( numeric_param_t::relative_xy);
    add_param( p);
}

void grid_node_t::do_process( const render::context_t& context)
{
	Imath::Color4f color( get_value<Imath::Color4f>( param( "bgcol")));
	boost::gil::fill_pixels( image_view(), image::pixel_t( color.r, color.g, color.b, color.a));
	
	Imath::V2f size( get_absolute_value<Imath::V2f>( param( "size")));
	Imath::V2f translate( get_absolute_value<Imath::V2f>( param( "translate")));
	Imath::V2f line_width( get_absolute_value<Imath::V2f>( param( "linewidth")));
	color = get_value<Imath::Color4f>( param( "fgcol"));
	
	// adjust params
	size.x = size.x / context.subsample / aspect_ratio();
	size.y /= context.subsample;

	if( size.x == 0 || size.y == 0)
		return;
	
	translate.x = translate.x / context.subsample / aspect_ratio();
	translate.y /= context.subsample;
	
	line_width.x = line_width.x / context.subsample / aspect_ratio();
	line_width.y /= context.subsample;
	
	if( line_width.x == 0 || line_width.y == 0)
		return;
	
	// setup agg
    typedef image::agg_rgba32f_renderer_t ren_base_type;
    typedef ren_base_type::color_type color_type;
    typedef agg::renderer_scanline_aa_solid<ren_base_type> renderer_type;
	
    ren_base_type ren_base( image_view());
    renderer_type ren( ren_base);

    agg::rasterizer_scanline_aa<> ras;
    ras.gamma( agg::gamma_none());

    agg::scanline_u8 sl;
	
	ras.reset();

	agg::path_storage path;
	agg::conv_stroke<agg::path_storage> stroke_conv( path);

	// Vertical
	stroke_conv.width( line_width.x);
	
	int w = image_view().width();
	int h = image_view().height();
	
	Imath::Box2f area( defined().min - translate, 
					   defined().max - translate);
	
	float x = Imath::Math<float>::floor( area.min.x / size.x) * size.x;
	for( ; x < area.max.x + line_width.x; x += size.x)
	{
		path.move_to( x - area.min.x, 0);
		path.line_to( x - area.min.x, h);
	}

	ras.add_path( stroke_conv);
    ren.color( image::pixel_t( color.r, color.g, color.b, color.a));
    agg::render_scanlines( ras, sl, ren);

	// Horizontal
	path.remove_all();
	stroke_conv.width( line_width.y);

	float y = Imath::Math<float>::floor( area.min.y / size.y) * size.y;
	for( ; y < area.max.y + line_width.y; y += size.y)
	{
		path.move_to( 0, y - area.min.y);
		path.line_to( w, y - area.min.y);
	}

	ras.add_path( stroke_conv);
    ren.color( image::pixel_t( color.r, color.g, color.b, color.a));
    agg::render_scanlines( ras, sl, ren);
}

// factory
node_t *create_grid_node() { return new grid_node_t();}

const node_metaclass_t *grid_node_t::metaclass() const { return &grid_node_metaclass();}

const node_metaclass_t& grid_node_t::grid_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.grid";
        info.major_version = 1;
        info.minor_version = 0;
        info.menu = "Image";
        info.submenu = "Input";
        info.menu_item = "Grid";
        info.create = &create_grid_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( grid_node_t::grid_node_metaclass());

} // namespace
} // namespace
