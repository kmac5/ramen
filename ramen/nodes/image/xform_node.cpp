// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/xform_node.hpp>

#include<ramen/ui/user_interface.hpp>

namespace ramen
{
namespace image
{

xform_node_t::xform_node_t() : base_warp_node_t() {}

xform_node_t::xform_node_t(const xform_node_t& other) : base_warp_node_t( other) {}

motion_blur_info_t::loop_data_t xform_node_t::motion_blur_loop_data( float frame, int extra_samples, float shutter_factor) const
{
	if( interacting())
		return motion_blur_info_t::loop_data_t( frame);

	const motion_blur_param_t *mb = 0;
	
	try
	{
	    const param_t *p = &param( "motion_blur");
	    mb = dynamic_cast<const motion_blur_param_t*>( p);
	}
	catch( std::exception& e) {}
	
	if( mb)
	    return mb->loop_data( frame, extra_samples, shutter_factor);
	else
		return motion_blur_info_t::loop_data_t( frame);	
}

void xform_node_t::create_motion_blur_param()
{
    std::auto_ptr<motion_blur_param_t> mb( new motion_blur_param_t( "Motion Blur"));
    mb->set_id( "motion_blur");
	mb->set_include_in_hash( false);
    add_param( mb);
}

void xform_node_t::do_calc_hash_str( const render::context_t& context)
{
    motion_blur_info_t::loop_data_t d( motion_blur_loop_data( context.frame, context.motion_blur_extra_samples,
                                                              context.motion_blur_shutter_factor));

	hash_generator() << ( int) get_filter_type();

	if( d.num_samples == 1)
		param_set().add_to_hash( hash_generator());
	else
	{
		float t = d.start_time;
	
		for( int i = 0; i < d.num_samples; ++i)
		{
			set_frame( t);
			param_set().add_to_hash( hash_generator());
			t += d.time_step;
		}
	
		set_frame( context.frame);
	}
}

bool xform_node_t::do_is_valid() const
{
    const image_node_t *src = get_img_source<xform_node_t>();

	if( src)
		return src->is_valid();

	return false;
}

void xform_node_t::do_begin_interaction()
{
	RAMEN_ASSERT( notify_pending_ == false);

	switch( get_filter_type())
	{
		case filter_point:
		case filter_bilinear:
		{
			notify_pending_ = false;

			const motion_blur_param_t *mb = 0;

			try
			{
			    const param_t *p = &param( "motion_blur");
			    mb = dynamic_cast<const motion_blur_param_t*>( p);
			}
			catch( std::exception& e) {}

			if( mb && mb->motion_blur_enabled())
				notify_pending_ = true;
		}
		break;

		default:
			notify_pending_ = true;
		break;
	}
}

} // image
} // ramen
