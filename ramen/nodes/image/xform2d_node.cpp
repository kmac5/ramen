// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/nodes/image/xform2d_node.hpp>

#include<vector>

#include<boost/foreach.hpp>

#include<ramen/ImathExt/ImathBoxAlgo.h>
#include<ramen/ImathExt/ImathMatrixAlgo.h>

#include<ramen/app/composition.hpp>

#include<ramen/image/processing.hpp>
#include<ramen/image/accumulator.hpp>

#include<ramen/image/generic/affine_warp.hpp>
#include<ramen/image/generic/projective_warp.hpp>

#include<ramen/ggems/convex_opt.h>

namespace ramen
{
namespace image
{

xform2d_node_t::xform2d_node_t() : xform_node_t() {}

xform2d_node_t::matrix3_type xform2d_node_t::calc_transform_matrix_at_frame( float frame, int subsample) const
{
    if( ignored())
        return matrix3_type();

    return do_calc_transform_matrix_at_frame( frame, subsample);
}

xform2d_node_t::matrix3_type xform2d_node_t::global_matrix_at_frame( float frame, const std::vector<const xform2d_node_t*>& xforms, int subsample) const
{
    matrix3_type global;
    global.makeIdentity();

    for( int i = xforms.size() - 1; i >= 0; --i)
    {
        matrix3_type local( xforms[i]->calc_transform_matrix_at_frame( frame, subsample));
        global *= local;
    }

    return global;
}

Imath::Box2i xform2d_node_t::transform_box( const Imath::Box2i& box, const matrix3_type& m, int step, bool round_up) const
{
	if( box.isEmpty())
		return box;
	
	if( isAffine( m))
		return Imath::transform( box, m, round_up);
	else
	{
		// TODO: this is really strange. The direct way of transforming the corners
		// does not work ( I don't know why).
		// For now, we transform all boundary points, just like we do in warp nodes.
		// Review this code later.
		
		Imath::Box<vector2_type> b;
		vector2_type p, q;
		
		for( int i = box.min.x; i <= box.max.x; ++i)
		{
			p = vector2_type( i, box.min.y);
			q = p * m;
			b.extendBy( q);

			p = vector2_type( i, box.max.y);
			q = p * m;
			b.extendBy( q);			
		}
	
		for( int i = box.min.y; i <= box.max.y; ++i)
		{
			p = vector2_type( box.min.x, i);
			q = p * m;
			b.extendBy( q);

			p = vector2_type( box.max.x, i);
			q = p * m;
			b.extendBy( q);
		}

		return Imath::roundBox( b, round_up);
	}
}

void xform2d_node_t::do_calc_bounds( const render::context_t& context)
{
    std::vector<const xform2d_node_t*> xforms;
    const image_node_t *src = get_img_source_and_xform_list<xform2d_node_t>( xforms);

    if( !src)
        return;

    motion_blur_info_t::loop_data_t d( motion_blur_loop_data( context.frame, context.motion_blur_extra_samples,
                                                                            context.motion_blur_shutter_factor));

    float t = d.start_time;
    Imath::Box2i area;

    for( int i = 0; i < d.num_samples; ++i)
    {
        matrix3_type m( global_matrix_at_frame( t, xforms));
		Imath::Box2i bounds( src->bounds());
		bounds.max.x++;
		bounds.max.y++;			
		bounds = transform_box( bounds, m, 50);
		area.extendBy( bounds);
        t += d.time_step;
    }

    set_bounds( area);
}

void xform2d_node_t::do_calc_inputs_interest( const render::context_t& context)
{
    if( interest().isEmpty())
        return;

    std::vector<const xform2d_node_t*> xforms;
    const image_node_t *src = get_img_source_and_xform_list<xform2d_node_t>( xforms);

    if( !src)
        return;

    motion_blur_info_t::loop_data_t d( motion_blur_loop_data( context.frame, context.motion_blur_extra_samples,
                                                                            context.motion_blur_shutter_factor));

    float t = d.start_time;
    Imath::Box2i roi;

    for( int i = 0; i < d.num_samples; ++i)
    {
        matrix3_type m( global_matrix_at_frame( t, xforms));

		try
		{
			matrix3_type inv_m( m.inverse( true));
			Imath::Box2i box( transform_box( interest(), inv_m, 50));
			roi.extendBy( box);
		}
		catch( Iex::MathExc& e) {}

        t += d.time_step;
    }

    if( !roi.isEmpty())
    {
		expand_interest( roi, context);
        const_cast<image_node_t*>( src)->add_interest( roi);
    }
}

void xform2d_node_t::do_recursive_process( const render::context_t& context)
{
    if( defined().isEmpty())
        return;

    image_node_t *src = get_img_source<xform2d_node_t>();

    if( src)
    {
        src->recursive_process( context);
        alloc_image();
        process( context);
        src->release_image();
    }
}

void xform2d_node_t::do_process( const render::context_t& context)
{
	if( get_filter_type() >= filter_mipmap)
	{
		do_process_mipmap( context);
		return;
	}

    std::vector<const xform2d_node_t*> xforms;
    const image_node_t *src = get_img_source_and_xform_list<xform2d_node_t>( xforms);

    motion_blur_info_t::loop_data_t d( motion_blur_loop_data( context.frame, context.motion_blur_extra_samples, context.motion_blur_shutter_factor));

    if( d.num_samples == 1)
    {
        matrix3_type xf( global_matrix_at_frame( context.frame, xforms, context.subsample));
        do_process( context, src, image_view(), xf);
    }
    else
    {
        image::rgba_accumulator_t acc( image_view());
		image::buffer_t tmp( image_view().width(), image_view().height(), 4);

        float t = d.start_time;
        float sumw = 0.0f;

        for( int i = 0; i < d.num_samples; ++i)
        {
            matrix3_type xf( global_matrix_at_frame( t, xforms, context.subsample));
            boost::gil::fill_pixels( tmp.rgba_view(), image::pixel_t( 0, 0, 0, 0));
            do_process( context, src, tmp.rgba_view(), xf);

            float w = d.weight_for_time( t);
            acc.accumulate( tmp.const_rgba_view(), w);
            t += d.time_step;
            sumw += w;
        }

        if( sumw != 0.0f)
            acc.multiply( 1.0f / sumw);
    }
}

void xform2d_node_t::do_process( const render::context_t& context, const image_node_t *src,
								 const image::image_view_t& dst, const matrix3_type& xf)
{
    if( src->defined().isEmpty())
        return;

    try
    {
        matrix3_type inv_xf = xf.inverse( true);

		filter_type filt = get_filter_type();

        if( isAffine( xf))
        {
            switch( filt)
            {
				case filter_point:
					image::affine_warp_nearest( src->defined(), src->const_image_view(), defined(), dst, xf, inv_xf);
				break;
	
				case filter_bilinear:
					image::affine_warp_bilinear( src->defined(), src->const_image_view(), defined(), dst, xf, inv_xf);
				break;
	
				case filter_catrom:
					image::affine_warp_catrom( src->defined(), src->const_image_view(), defined(), dst, xf, inv_xf);
				break;
            }
        }
        else
        {
            switch( filt)
            {
				case filter_point:
					image::projective_warp_nearest( src->defined(), src->const_image_view(), defined(), dst, xf, inv_xf);
				break;
	
				case filter_bilinear:
					image::projective_warp_bilinear( src->defined(), src->const_image_view(), defined(), dst, xf, inv_xf);
				break;
	
				case filter_catrom:
					image::projective_warp_catrom( src->defined(), src->const_image_view(), defined(), dst, xf, inv_xf);
				break;
            }
        }
    }
    catch( Iex::MathExc)
    {
    }
}

void xform2d_node_t::do_process_mipmap( const render::context_t& context)
{
    std::vector<const xform2d_node_t*> xforms;
    const image_node_t *src = get_img_source_and_xform_list<xform2d_node_t>( xforms);

	std::vector<image::buffer_t> buffers;
	mipmap_type sampler;
	make_mipmap( const_cast<image_node_t*>( src), sampler, buffers);

    motion_blur_info_t::loop_data_t d( motion_blur_loop_data( context.frame, context.motion_blur_extra_samples, context.motion_blur_shutter_factor));

    if( d.num_samples == 1)
    {
        matrix3_type xf( global_matrix_at_frame( context.frame, xforms, context.subsample));
        do_process_mipmap( context, src, image_view(), xf, sampler);
    }
    else
    {
        image::rgba_accumulator_t acc( image_view());
		image::buffer_t tmp( image_view().width(), image_view().height(), 4);

        float t = d.start_time;
        float sumw = 0.0f;

        for( int i = 0; i < d.num_samples; ++i)
        {
            matrix3_type xf( global_matrix_at_frame( t, xforms, context.subsample));
            boost::gil::fill_pixels( tmp.rgba_view(), image::pixel_t( 0, 0, 0, 0));
            do_process_mipmap( context, src, tmp.rgba_view(), xf, sampler);

            float w = d.weight_for_time( t);
            acc.accumulate( tmp.const_rgba_view(), w);
            t += d.time_step;
            sumw += w;
        }

        if( sumw != 0.0f)
            acc.multiply( 1.0f / sumw);
    }
}

void xform2d_node_t::do_process_mipmap( const render::context_t& context, const image_node_t *src, const image::image_view_t& dst,
										const matrix3_type& xf, mipmap_type& sampler)
{
    if( src->defined().isEmpty())
        return;

    try
    {
        matrix3_type inv_xf = xf.inverse( true);

		filter_type filt = get_filter_type();

        if( isAffine( xf))
        {
            switch( filt)
            {
				case filter_mipmap:
					image::generic::affine_warp( sampler, src->defined(), defined(), dst, xf, inv_xf);
				break;

				default:
					RAMEN_ASSERT( 0);
				break;
            }
        }
        else
        {
            switch( filt)
            {
				case filter_mipmap:
					image::generic::projective_warp( sampler, defined(), dst, xf, inv_xf, true);
				break;

				default:
					RAMEN_ASSERT( 0);
				break;
            }
        }
    }
    catch( Iex::MathExc)
    {
    }
}

bool xform2d_node_t::quad_is_convex(const boost::array<vector2_type,4>& quad) const
{
    float verts[4][2];

    for( int i = 0; i < 4; ++i)
    {
        verts[i][0] = quad[i].x;
        verts[i][1] = quad[i].y;
    }

    PolygonClass result = classifyPolygon2( 4, verts);
    return ( result == ConvexCW) || ( result == ConvexCCW);
}

xform2d_node_t::matrix3_type xform2d_node_t::global_matrix( int subsample) const
{
	RAMEN_ASSERT( composition());

	float frame = composition()->frame();
	return global_matrix_at_frame( frame, subsample);
}

xform2d_node_t::matrix3_type xform2d_node_t::global_matrix_at_frame( float frame, int subsample) const
{
    std::vector<const xform2d_node_t*> xforms;
    const image_node_t *src = get_img_source_and_xform_list<xform2d_node_t>( xforms);
	return global_matrix_at_frame( frame, xforms, subsample);
}

} // namespace
} // namespace
