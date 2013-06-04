// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/lens_distort/lens_distort_node.hpp>

#include<OpenEXR/ImathFun.h>

#include<ramen/params/combo_group_param.hpp>
#include<ramen/params/float_param.hpp>
#include<ramen/params/popup_param.hpp>

#include<ramen/image/processing.hpp>

#include<ramen/camera/syntheyes_lens_distortion.hpp>

namespace ramen
{
namespace image
{
namespace
{

enum
{
	undistort = 0,
	redistort
};

enum
{
	syntheyes_model = 0
};

enum
{
	bilinear = 0,
	catrom
};

} // namespace

lens_distort_node_t::lens_distort_node_t() : image_node_t()
{
    add_input_plug( "front", false, ui::palette_t::instance().color( "front plug"), "Front");
    add_output_plug();
	set_name( "lens_dist");
}

lens_distort_node_t::lens_distort_node_t( const lens_distort_node_t& other) : image_node_t( other) {}

void lens_distort_node_t::do_create_params()
{
	std::auto_ptr<popup_param_t> mode( new popup_param_t( "Mode"));
	mode->set_id( "mode");
	mode->menu_items() = boost::assign::list_of( "Undistort")( "Redistort");
	add_param( mode);

    std::auto_ptr<popup_param_t> filter( new popup_param_t( "Filter"));
    filter->set_id( "filter");
    filter->menu_items() = boost::assign::list_of( "Bilinear")( "Catrom");
    filter->set_default_value( 0);
    add_param( filter);
		
    std::auto_ptr<combo_group_param_t> top( new combo_group_param_t( "Model"));
    top->set_id( "model");
    {
		std::auto_ptr<composite_param_t> group( new composite_param_t( "Syntheyes"));
		group->set_id( "se");

		std::auto_ptr<float_param_t> p( new float_param_t( "Distortion"));
		p->set_id( "synth_k");
		p->set_static( true);
		p->set_default_value( 0.0f);
		p->set_range( -1, 1);
		p->set_step( 0.01);
		group->add_param( p);

		p.reset( new float_param_t( "Cubic Dist."));
		p->set_id( "synth_k3");
		p->set_static( true);
		p->set_default_value( 0.0f);
		p->set_range( -5, 5);
		p->set_step( 0.01);
		group->add_param( p);
		
		top->add_param( group);
	}
	add_param( top);
}

void lens_distort_node_t::do_calc_bounds( const render::context_t& context)
{
	image_node_t *in = input_as<image_node_t>();
	Imath::Box2i b = in->bounds();
	
	switch( get_value<int>( param( "model")))
	{
		case syntheyes_model:
		{
			if( get_value<int>( param( "mode")) == undistort)
				b = redistort_box( b);
			else
				b = undistort_box( b);
		}
		break;
	};

	set_bounds( b);
}

void lens_distort_node_t::do_calc_inputs_interest( const render::context_t& context)
{
	image_node_t *in = input_as<image_node_t>();	 
	Imath::Box2i roi = Imath::intersect( interest(), bounds());

	switch( get_value<int>( param( "model")))
	{
		case syntheyes_model:
		{
			if( get_value<int>( param( "mode")) == undistort)
				roi = undistort_box( roi);
			else
				roi = redistort_box( roi);
		}
		break;
	};
	
	// add a small margin for filtering
	roi.min.x -= 1;
	roi.min.y -= 1;
	
	if( get_value<int>( param( "filter")) == bilinear)
	{
		roi.max.x += 1;
		roi.max.y += 1;
	}
	else
	{
		roi.max.x += 3;
		roi.max.y += 3;
	}

	in->add_interest( roi);
}

void lens_distort_node_t::do_process( const render::context_t& context)
{
	image_node_t *in = input_as<image_node_t>();
	
	switch( get_value<int>( param( "model")))
	{
		case syntheyes_model:
		{
			float k = get_value<float>( param( "synth_k"));
			float k3 = get_value<float>( param( "synth_k3"));
			
			if( get_value<int>( param( "mode")) == undistort)
			{
				if( k3 == 0)
				{
					camera::syntheyes_quadratic_undistort dist( k, in->format(), in->aspect_ratio());

					if( get_value<int>( param( "filter")) == bilinear)
						image::warp_bilinear( in->defined(), in->const_image_view(), defined(), image_view(), dist, false, false);
					else
						image::warp_bicubic( in->defined(), in->const_image_view(), defined(), image_view(), dist, false, false);
				}
				else
				{
					camera::syntheyes_undistort dist( k, k3, in->format(), in->aspect_ratio());
					
					if( get_value<int>( param( "filter")) == bilinear)
						image::warp_bilinear( in->defined(), in->const_image_view(), defined(), image_view(), dist, false, false);
					else
						image::warp_bicubic( in->defined(), in->const_image_view(), defined(), image_view(), dist, false, false);
				}
			}
			else
			{
				if( k3 == 0)
				{
					camera::syntheyes_quadratic_redistort dist( k, in->format(), in->aspect_ratio());

					if( get_value<int>( param( "filter")) == bilinear)
						image::warp_bilinear( in->defined(), in->const_image_view(), defined(), image_view(), dist, false, false);
					else
						image::warp_bicubic( in->defined(), in->const_image_view(), defined(), image_view(), dist, false, false);
				}
				else
				{
					camera::syntheyes_redistort dist( k, k3, in->format(), in->aspect_ratio());
					
					if( get_value<int>( param( "filter")) == bilinear)
						image::warp_bilinear( in->defined(), in->const_image_view(), defined(), image_view(), dist, false, false);
					else
						image::warp_bicubic( in->defined(), in->const_image_view(), defined(), image_view(), dist, false, false);
				}
			}
		}
		break;
	};
}

Imath::Box2i lens_distort_node_t::undistort_box( const Imath::Box2i& b) const
{
	switch( get_value<int>( param( "model")))
	{
		case syntheyes_model:
		{
			float k = get_value<float>( param( "synth_k"));
			float k3 = get_value<float>( param( "synth_k3"));
			Imath::V2f center = format().center();
			
			if( k3 == 0)
			{
				camera::syntheyes_quadratic_undistort dist( k, format(), aspect_ratio());
				return distort_box( b, center, dist);
			}
			else
			{
				camera::syntheyes_undistort dist( k, k3, format(), aspect_ratio());
				return distort_box( b, center, dist);
			}
		}
		break;
	};
}

Imath::Box2i lens_distort_node_t::redistort_box( const Imath::Box2i& b) const
{
	switch( get_value<int>( param( "model")))
	{
		case syntheyes_model:
		{
			float k = get_value<float>( param( "synth_k"));
			float k3 = get_value<float>( param( "synth_k3"));			
			Imath::V2f center = format().center();

			if( k3 == 0)
			{
				camera::syntheyes_quadratic_redistort dist( k, format(), aspect_ratio());
				return distort_box( b, center, dist);
			}
			else
			{
				camera::syntheyes_redistort dist( k, k3, format(), aspect_ratio());
				return distort_box( b, center, dist);
			}
		}
		break;
	};
}

// factory
node_t *create_lens_distort_node() { return new lens_distort_node_t();}

const node_metaclass_t *lens_distort_node_t::metaclass() const { return &lens_distort_node_metaclass();}

const node_metaclass_t& lens_distort_node_t::lens_distort_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
		info.id = "image.builtin.lens_distort";
		info.major_version = 1;
		info.minor_version = 0;
		info.menu = "Image";
		info.submenu = "Distort";
		info.menu_item = "Lens Distort";
        info.help = "Adds or removes lens distortion from an image.";
		info.create = &create_lens_distort_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( lens_distort_node_t::lens_distort_node_metaclass());

} // namespace
} // namespace

