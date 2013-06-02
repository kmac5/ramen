// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/layer/alpha_layer_node.hpp>

#include<ramen/params/popup_param.hpp>
#include<ramen/params/float_param.hpp>

#include<ramen/image/processing.hpp>

namespace ramen
{
namespace image
{
namespace
{

enum
{
    comp_add = 0,
    comp_mult,
    comp_sub,
    comp_mix,
    comp_max,
    comp_min
};

} // namespace

alpha_layer_node_t::alpha_layer_node_t() : base_layer_node_t()
{
    set_name( "layer");
    add_input_plug( "back", false, ui::palette_t::instance().color( "back plug"), "Back");
    add_input_plug( "front", false, ui::palette_t::instance().color( "front plug"), "Front");
    add_output_plug();
}

void alpha_layer_node_t::do_create_params()
{
    std::auto_ptr<popup_param_t> p( new popup_param_t( "Layer Mode"));
    p->set_id( "layer_mode");
    p->menu_items() = boost::assign::list_of( "Add")( "Mult")( "Sub")( "Mix")( "Max")( "Min");
    add_param( p);

    std::auto_ptr<float_param_t> q( new float_param_t( "Opacity"));
    q->set_id( "opacity");
    q->set_range( 0, 1);
    q->set_default_value( 1);
    q->set_step( 0.01);
    add_param( q);
}

void alpha_layer_node_t::do_calc_format( const render::context_t& context)
{
	image_node_t *in = input_as<image_node_t>( 0);
	set_format( in->format());
	set_aspect_ratio( in->aspect_ratio());
	set_proxy_scale( in->proxy_scale());
}

void alpha_layer_node_t::do_calc_bounds( const render::context_t& context)
{
    Imath::Box2i bbox;

    float opacity = get_value<float>( param( "opacity"));

    if( opacity == 1.0f)
    {
        switch( get_value<int>( param( "layer_mode")))
        {
        case comp_mult:
        case comp_min:
        case comp_mix:
            bbox = Imath::intersect( input_as<image_node_t>( 0)->bounds(), input_as<image_node_t>( 1)->bounds());
        break;

        case comp_sub:
            bbox = input_as<image_node_t>( 0)->bounds();
        break;

        default:
            bbox = input_as<image_node_t>( 0)->bounds();
            bbox.extendBy( input_as<image_node_t>( 1)->bounds());
        }
    }
    else
    {
        bbox = input_as<image_node_t>( 0)->bounds();
        bbox.extendBy( input_as<image_node_t>( 1)->bounds());
    }

    set_bounds( bbox);
}

void alpha_layer_node_t::do_process( const render::context_t& context)
{
    int mode = get_value<int>( param( "layer_mode"));

    if( mode == comp_mult || mode == comp_min || mode == comp_mix) // min, mult and mix are special
    {
        do_process_mult_min_mix( context);
        return;
    }

    image_node_t *bg = input_as<image_node_t>( 0);
    image_node_t *fg = input_as<image_node_t>( 1);

    Imath::Box2i bg_area = Imath::intersect( bg->defined(), defined());
    float opacity = get_value<float>( param( "opacity"));

    if( !bg_area.isEmpty())
    {
        render_input( 0, context);
        boost::gil::copy_pixels( bg->const_subimage_view( bg_area), subimage_view( bg_area));
        release_input_image( 0);
    }

    Imath::Box2i comp_area( Imath::intersect( fg->defined(), defined()));

    if( !comp_area.isEmpty())
    {
        if( opacity == 0.0f)
            return;

        render_input( 1, context);

        switch( mode)
        {
        case comp_add:
            image::alpha_composite_add( const_subimage_view( comp_area), 
										input_as<image_node_t>( 1)->const_subimage_view( comp_area), 
										subimage_view( comp_area), opacity);
        break;

        case comp_sub:
            image::alpha_composite_sub( const_subimage_view( comp_area), 
										input_as<image_node_t>( 1)->const_subimage_view( comp_area), 
										subimage_view( comp_area), opacity);
        break;

        case comp_max:
            image::alpha_composite_max( const_subimage_view( comp_area), 
										input_as<image_node_t>( 1)->const_subimage_view( comp_area), 
										subimage_view( comp_area), opacity);
        break;
        }

        release_input_image( 1);
    }
}

void alpha_layer_node_t::do_process_mult_min_mix( const render::context_t& context)
{
    image_node_t *bg = input_as<image_node_t>( 0);
    image_node_t *fg = input_as<image_node_t>( 1);

    int mode = get_value<int>( param( "layer_mode"));
    float opacity = get_value<float>( param( "opacity"));

    Imath::Box2i bg_area(   Imath::intersect( bg->defined(), defined()));
    Imath::Box2i comp_area( Imath::intersect( fg->defined(), defined()));

    if( !bg_area.isEmpty())
    {
        render_input( 0, context);

        if( opacity == 0.0f) // just copy the background and return
        {
            boost::gil::copy_pixels( bg->const_subimage_view( bg_area), subimage_view( bg_area));
            return;
        }

        if( opacity == 1.0f) // the normal case, nothing special to do
			boost::gil::copy_pixels( bg->const_subimage_view( bg_area), subimage_view( bg_area));
		else // we need to handle the areas of the bg that don't intersect the fg
		{
			image::mul_channel_scalar( boost::gil::nth_channel_view( bg->const_subimage_view( bg_area), 3), 1.0f - opacity, 
									   boost::gil::nth_channel_view( subimage_view( bg_area), 3));
			
			Imath::Box2i common_area( Imath::intersect( fg->defined(), bg->defined()));
			
			if( !common_area.isEmpty())
				boost::gil::copy_pixels( bg->const_subimage_view( common_area), subimage_view( common_area));
		}
		
        // we don't need the bg anymore
        release_input_image( 0);
    }

    if( !comp_area.isEmpty())
    {
        render_input( 1, context);

        switch( mode)
        {
        case comp_min:
            image::alpha_composite_min( const_subimage_view( comp_area), 
										input_as<image_node_t>( 1)->const_subimage_view( comp_area), 
										subimage_view( comp_area), opacity);
        break;

        case comp_mult:
            image::alpha_composite_mul( const_subimage_view( comp_area), 
										input_as<image_node_t>( 1)->const_subimage_view( comp_area), 
										subimage_view( comp_area), opacity);
        break;

        case comp_mix:
            image::alpha_composite_mix( const_subimage_view( comp_area), 
										input_as<image_node_t>( 1)->const_subimage_view( comp_area), 
										subimage_view( comp_area), opacity);
        break;
        }

        release_input_image( 1);
    }
}

// factory
node_t *create_alpha_layer_node() { return new alpha_layer_node_t();}

const node_metaclass_t *alpha_layer_node_t::metaclass() const { return &alpha_layer_node_metaclass();}

const node_metaclass_t& alpha_layer_node_t::alpha_layer_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.alpha_layer";
        info.major_version = 1;
        info.minor_version = 0;
        info.menu = "Image";
        info.submenu = "Layer";
        info.menu_item = "Alpha Layer";
        info.create = &create_alpha_layer_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( alpha_layer_node_t::alpha_layer_node_metaclass());

} // namespace
} // namespace
