// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/nodes/image/generate/checkerboard_node.hpp>

#include<OpenEXR/ImathMath.h>

#include<agg2/agg_renderer_scanline.h>
#include<agg2/agg_rasterizer_scanline_aa.h>
#include<agg2/agg_scanline_u.h>
#include<agg2/agg_path_storage.h>

#include<ramen/nodes/node_factory.hpp>

#include<ramen/params/float2_param.hpp>
#include<ramen/params/color_param.hpp>

#include<ramen/image/agg_renderers.hpp>

namespace ramen
{
namespace image
{

checkerboard_node_t::checkerboard_node_t() : generator_node_t() { set_name( "checks");}

void checkerboard_node_t::do_create_params()
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

    p.reset( new float2_param_t( "Translate"));
    p->set_id( "translate");
    p->set_default_value( Imath::V2f( 0, 0));
    p->set_numeric_type( numeric_param_t::relative_xy);
    add_param( p);
}

void checkerboard_node_t::do_process( const render::context_t& context)
{
	Imath::Color4f color( get_value<Imath::Color4f>( param( "bgcol")));
	boost::gil::fill_pixels( image_view(), image::pixel_t( color.r, color.g, color.b, color.a));
	
	Imath::V2f size( get_absolute_value<Imath::V2f>( param( "size")));
	
	if( size.x == 0 || size.y == 0)
		return;
	
	Imath::V2f translate( get_absolute_value<Imath::V2f>( param( "translate")));
	color = get_value<Imath::Color4f>( param( "fgcol"));
	
	// adjust params
	size.x = size.x / context.subsample / aspect_ratio();
	size.y /= context.subsample;

	translate.x = translate.x / context.subsample / aspect_ratio();
	translate.y /= context.subsample;
	
	// setup agg
    typedef image::agg_rgba32f_renderer_t ren_base_type;
    typedef ren_base_type::color_type color_type;
    typedef agg::renderer_scanline_aa_solid<ren_base_type> renderer_type;
	
    ren_base_type ren_base( image_view());
    renderer_type ren( ren_base);

    agg::rasterizer_scanline_aa<> ras;
    ras.gamma( agg::gamma_none());
	ras.reset();

    agg::scanline_u8 sl;
	agg::path_storage path;

	int w = image_view().width();
	int h = image_view().height();
	
	Imath::Box2f area( defined().min - translate, 
					   defined().max - translate);

	int ix = Imath::Math<float>::floor( area.min.x / size.x);
	int iy = Imath::Math<float>::floor( area.min.y / size.y);

	for( float y = iy * size.y; y < area.max.y; y += size.y, ++iy)
	{
		int iix = ix;
		for( float x = ix * size.x; x < area.max.x; x += size.x, ++iix)
		{
			if( ( iy & 1) != ( iix & 1))
			{
				path.remove_all();
				path.move_to( x - area.min.x			, y - area.min.y);
				path.line_to( x - area.min.x + size.x	, y - area.min.y);
				path.line_to( x - area.min.x + size.x	, y - area.min.y + size.y);
				path.line_to( x - area.min.x			, y - area.min.y + size.y);
				path.end_poly();
				
				ras.add_path( path);
				ren.color( image::pixel_t( color.r, color.g, color.b, color.a));
				agg::render_scanlines( ras, sl, ren);
			}
		}
	}
}

// factory
node_t *create_checkerboard_node() { return new checkerboard_node_t();}

const node_metaclass_t *checkerboard_node_t::metaclass() const { return &checkerboard_node_metaclass();}

const node_metaclass_t& checkerboard_node_t::checkerboard_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.checkerboard";
        info.major_version = 1;
        info.minor_version = 0;
        info.menu = "Image";
        info.submenu = "Input";
        info.menu_item = "Checkerboard";
        info.create = &create_checkerboard_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( checkerboard_node_t::checkerboard_node_metaclass());

} // namespace
} // namespace
