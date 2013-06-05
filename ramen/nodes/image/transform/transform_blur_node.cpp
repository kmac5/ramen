// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/transform/transform_blur_node.hpp>

#include<ramen/assert.hpp>

#include<ramen/ImathExt/ImathBoxAlgo.h>

#include<ramen/params/transform2_param.hpp>
#include<ramen/params/separator_param.hpp>
#include<ramen/params/float_param.hpp>
#include<ramen/params/popup_param.hpp>

#include<ramen/image/processing.hpp>

#include<ramen/image/accumulator.hpp>

namespace ramen
{
namespace
{

enum
{
    border_black  = 0,
    border_tile   = 1,
    border_mirror = 2
};

} // unnamed

transform_blur_node_t::transform_blur_node_t() : image_node_t()
{
    set_name("move2_blur");
    add_input_plug( "front", false, ui::palette_t::instance().color("front plug"), "Front");
    add_output_plug();
}

void transform_blur_node_t::do_create_params()
{
    std::auto_ptr<transform2_param_t> xp( new transform2_param_t( "xform", "xf"));
    add_param( xp);

    std::auto_ptr<separator_param_t> sep( new separator_param_t());
    add_param( sep);

    std::auto_ptr<float_param_t> p( new float_param_t( "Samples"));
    p->set_id( "samples");
	p->set_static( true);
    p->set_min( 1);
    p->set_default_value( 1);
    p->set_round_to_int( true);
    add_param( p);

    std::auto_ptr<popup_param_t> q( new popup_param_t( "Borders"));
    q->set_id( "borders");
    q->menu_items() = boost::assign::list_of( "Black")( "Tile")( "Mirror");
    add_param( q);
}

void transform_blur_node_t::do_calc_bounds( const render::context_t& context)
{
    transform2_param_t *p = dynamic_cast<transform2_param_t*>( &param( "xf"));
    RAMEN_ASSERT( p);

    int num_samples = ( get_value<float>( param( "samples")) * 2) + 1;

    Imath::Box2i area;

    for( int i = 0; i < num_samples; ++i)
    {
        Imath::M33d m( p->xform_blur_matrix_at_frame( context.frame, (float) i / ( num_samples - 1) , aspect_ratio(), context.subsample));
		Imath::Box2i bounds( ImathExt::transform( input_as<image_node_t>()->bounds(), m, false));
		area.extendBy( bounds);
    }

    set_bounds( area);
}

void transform_blur_node_t::do_calc_inputs_interest( const render::context_t& context)
{
    if( interest().isEmpty())
        return;

    transform2_param_t *p = dynamic_cast<transform2_param_t*>( &param( "xf"));
    RAMEN_ASSERT( p);

    int num_samples = ( get_value<float>( param( "samples")) * 2) + 1;

    Imath::Box2i roi;

    for( int i = 0; i < num_samples; ++i)
    {
        Imath::M33d m( p->xform_blur_matrix_at_frame( context.frame, (float) i / ( num_samples - 1), aspect_ratio(), context.subsample));
		
		try
		{
			Imath::M33d inv_m( m.inverse( true));
			Imath::Box2i box( ImathExt::transform( interest(), inv_m, false));
			roi.extendBy( box);
		}
		catch( Iex::MathExc& e)
		{
		}       
    }

    if( !roi.isEmpty())
    {
        // add some margin for filtering
        roi.min.x -= 2 * context.subsample;
        roi.min.y -= 2 * context.subsample;
        roi.max.x += 2 * context.subsample;
        roi.max.y += 2 * context.subsample;
        input_as<image_node_t>()->add_interest( roi);
    }
}

void transform_blur_node_t::do_process( const render::context_t& context)
{
    transform2_param_t *p = dynamic_cast<transform2_param_t*>( &param( "xf"));
    RAMEN_ASSERT( p);

    int num_samples = ( get_value<float>( param( "samples")) * 2) + 1;
    int border_mode = get_value<int>( param( "borders"));

    image::rgba_accumulator_t acc( image_view());

    image::image_t tmp( image_view().width(), image_view().height());

    float sumw = 0.0f;

    for( int i = 0; i < num_samples; ++i)
    {
        Imath::M33d m( p->xform_blur_matrix_at_frame( context.frame, (float) i / ( num_samples - 1), aspect_ratio(), context.subsample));
        boost::gil::fill_pixels( boost::gil::view( tmp), image::pixel_t( 0, 0, 0, 0));

        do_process( boost::gil::view( tmp), m, border_mode);

        float w = 1.0f;
        acc.accumulate( boost::gil::view( tmp), w);

        sumw += w;
    }

    if( sumw != 0.0f)
        acc.multiply( 1.0f / sumw);
}

void transform_blur_node_t::do_process( const image::image_view_t& dst, const Imath::M33d& xf, int border_mode)
{
    if( input_as<image_node_t>()->defined().isEmpty())
        return;

    try
    {
        Imath::M33d inv_xf = xf.inverse( true);

        if( border_mode == border_black)
		{
            image::affine_warp_bilinear( input_as<image_node_t>()->defined(),
										 input_as<image_node_t>()->const_image_view(),
										 defined(), dst, xf, inv_xf);
		}
        else
        {
            if( border_mode == border_tile)
			{
                image::affine_warp_bilinear_tile( input_as<image_node_t>()->defined(),
												  input_as<image_node_t>()->const_image_view(),
												  defined(), dst, xf, inv_xf);
			}
            else
			{
                image::affine_warp_bilinear_mirror( input_as<image_node_t>()->defined(),
													input_as<image_node_t>()->const_image_view(),
													defined(), dst, xf, inv_xf);
			}
        }
    }
    catch( Iex::MathExc)
    {
    }
}

// factory

node_t *create_transform_blur_node() { return new transform_blur_node_t();}

const node_metaclass_t *transform_blur_node_t::metaclass() const { return &transform_blur_node_metaclass();}

const node_metaclass_t& transform_blur_node_t::transform_blur_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.transform_blur";
        info.major_version = 1;
        info.minor_version = 0;
		info.menu = "Image";
        info.submenu = "Transform";
        info.menu_item = "Transform Blur";
        info.create = &create_transform_blur_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( transform_blur_node_t::transform_blur_node_metaclass());

} // namespace
