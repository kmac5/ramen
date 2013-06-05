// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/transform/crop_node.hpp>

#include<tbb/blocked_range.h>
#include<tbb/parallel_for.h>

#include<ramen/params/float_param.hpp>
#include<ramen/params/bool_param.hpp>
#include<ramen/params/separator_param.hpp>

namespace ramen
{
namespace image
{
namespace
{

struct alpha_soft_crop_fun
{
	alpha_soft_crop_fun( const image::const_image_view_t& src, const image::image_view_t& dst, const Imath::Box2i& format, 
				   const Imath::Box2i& area, float t, float l, float b, float r) : src_( src), dst_( dst)
	{
		format_ = format;
		area_ = area;
		t_ = t;
		l_ = l;
		b_ = b;
		r_ = r;
	}

    void operator()( const tbb::blocked_range<int>& r) const
    {
		for( int j = r.begin(); j < r.end(); ++j)
		{
            image::const_image_view_t::x_iterator src_it( src_.row_begin( j));
            image::image_view_t::x_iterator dst_it( dst_.row_begin( j));
						
			for( int i = 0; i < src_.width(); ++i)
			{
				image::pixel_t p( *src_it++);
				boost::gil::get_color( p, boost::gil::alpha_t()) *= softness( i + area_.min.x, j + area_.min.y);				
				*dst_it++ = p;
			}
		}
	}
	
protected:

	float softness( int x, int y) const
	{
		float s = 1.0f;

		// top
		if( t_ != 0 && y - format_.min.y < t_)
			s *= ( y - format_.min.y) / t_;

		// bottom
		if( b_ != 0 && format_.max.y - y < b_)
			s *= ( format_.max.y - y) / b_;

		// left
		if( l_ != 0 && x - format_.min.x < l_)
			s *= ( x - format_.min.x) / l_;

		// right
		if( r_ != 0 && format_.max.x - x < r_)
			s *= ( format_.max.x - x) / r_;
		
		return s;
	}
	
	image::const_image_view_t src_;
	image::image_view_t dst_;
	
	Imath::Box2i format_, area_;
	
	float t_, l_, b_, r_;
};

struct rgba_soft_crop_fun : public alpha_soft_crop_fun
{
	rgba_soft_crop_fun( const image::const_image_view_t& src, const image::image_view_t& dst, const Imath::Box2i& format, 
						   const Imath::Box2i& area, float t, float l, float b, float r) : alpha_soft_crop_fun( src, dst, format, area,
																												t, l, b, r)
	{
	}

    void operator()( const tbb::blocked_range<int>& r) const
    {
		for( int j = r.begin(); j < r.end(); ++j)
		{
            image::const_image_view_t::x_iterator src_it( src_.row_begin( j));
            image::image_view_t::x_iterator dst_it( dst_.row_begin( j));
						
			for( int i = 0; i < src_.width(); ++i)
			{
				image::pixel_t p( *src_it++);
				float s = softness( i + area_.min.x, j + area_.min.y);
				boost::gil::get_color( p, boost::gil::red_t())   *= s;
				boost::gil::get_color( p, boost::gil::green_t()) *= s;
				boost::gil::get_color( p, boost::gil::blue_t())  *= s;
				boost::gil::get_color( p, boost::gil::alpha_t()) *= s;
				*dst_it++ = p;
			}
		}
	}
};

} // unnamed

crop_node_t::crop_node_t() : image_node_t()
{
    set_name("crop");
    add_input_plug( "front", false, ui::palette_t::instance().color("front plug"), "Front");
    add_output_plug();
}

void crop_node_t::do_create_params()
{
    std::auto_ptr<float_param_t> p( new float_param_t( "Top"));
    p->set_id( "top");
    p->set_range( 0, 1);
    p->set_numeric_type( numeric_param_t::relative_size_y, 0);
    p->set_default_value( 0);
    p->set_static( true);
    add_param( p);

    p.reset( new float_param_t( "Left"));
    p->set_id( "left");
    p->set_range( 0, 1);
    p->set_numeric_type( numeric_param_t::relative_size_x, 0);
    p->set_default_value( 0);
    p->set_static( true);
    add_param( p);

    p.reset( new float_param_t( "Bottom"));
    p->set_id( "bottom");
    p->set_range( 0, 1);
    p->set_numeric_type( numeric_param_t::relative_size_y, 0);
    p->set_default_value( 0);
    p->set_static( true);
    add_param( p);

    p.reset( new float_param_t( "Right"));
    p->set_id( "right");
    p->set_range( 0, 1);
    p->set_numeric_type( numeric_param_t::relative_size_x, 0);
    p->set_default_value( 0);
    p->set_static( true);
    add_param( p);
	
	std::auto_ptr<separator_param_t> sep( new separator_param_t());
	add_param( sep);
	
    p.reset( new float_param_t( "Soft Top"));
    p->set_id( "soft_top");
    p->set_range( 0, 1);
    p->set_numeric_type( numeric_param_t::relative_size_y, 0);
    p->set_default_value( 0);
    add_param( p);

    p.reset( new float_param_t( "Soft Left"));
    p->set_id( "soft_left");
    p->set_range( 0, 1);
    p->set_numeric_type( numeric_param_t::relative_size_x, 0);
    p->set_default_value( 0);
    add_param( p);

    p.reset( new float_param_t( "Soft Bottom"));
    p->set_id( "soft_bottom");
    p->set_range( 0, 1);
    p->set_numeric_type( numeric_param_t::relative_size_y, 0);
    p->set_default_value( 0);
    add_param( p);

    p.reset( new float_param_t( "Soft Right"));
    p->set_id( "soft_right");
    p->set_range( 0, 1);
    p->set_numeric_type( numeric_param_t::relative_size_x, 0);
    p->set_default_value( 0);
    add_param( p);
	
	std::auto_ptr<bool_param_t> b( new bool_param_t( "Alpha only"));
	b->set_id( "alpha_only");
	b->set_default_value( false);
	add_param( b);
}

void crop_node_t::do_calc_format( const render::context_t& context)
{
	image_node_t *in = input_as<image_node_t>();
    Imath::Box2i box( in->format());
    int crop_t = get_absolute_value<float>( param( "top"));
    int crop_l = get_absolute_value<float>( param( "left"));
    int crop_b = get_absolute_value<float>( param( "bottom"));
    int crop_r = get_absolute_value<float>( param( "right"));
    set_format( Imath::Box2i( Imath::V2i( box.min.x + crop_l, box.min.y + crop_t), Imath::V2i( box.max.x - crop_r, box.max.y - crop_b)));
	set_aspect_ratio( in->aspect_ratio());
	set_proxy_scale( in->proxy_scale());
}

void crop_node_t::do_calc_bounds( const render::context_t& context)
{
    Imath::Box2i bounds( ImathExt::intersect( input_as<image_node_t>()->bounds(), format()));
    set_bounds( bounds);
}

void crop_node_t::do_calc_inputs_interest( const render::context_t& context)
{
    Imath::Box2i roi = ImathExt::intersect( interest(), format());
    input_as<image_node_t>()->add_interest( roi);
}

void crop_node_t::do_process( const render::context_t& context)
{
	image_node_t *in = input_as<image_node_t>();
    Imath::Box2i area = ImathExt::intersect( in->defined(), defined());

    if( area.isEmpty())
		return;

    float soft_t = get_absolute_value<float>( param( "soft_top"));
    float soft_l = get_absolute_value<float>( param( "soft_left"));
    float soft_b = get_absolute_value<float>( param( "soft_bottom"));
    float soft_r = get_absolute_value<float>( param( "soft_right"));

	if( soft_t == 0 && soft_l == 0 && soft_b == 0 && soft_r == 0)
	    boost::gil::copy_pixels( input_as<image_node_t>()->const_subimage_view( area), subimage_view( area));
	else
	{
		if( get_value<bool>( param( "alpha_only")))
		{
			tbb::parallel_for( tbb::blocked_range<int>( 0, area.size().y + 1),
							   alpha_soft_crop_fun( in->const_subimage_view( area), subimage_view( area),
													format(), area, soft_t, soft_l, soft_b, soft_r), tbb::auto_partitioner());
		}
		else
		{
			tbb::parallel_for( tbb::blocked_range<int>( 0, area.size().y + 1),
							   rgba_soft_crop_fun( in->const_subimage_view( area), subimage_view( area),
													format(), area, soft_t, soft_l, soft_b, soft_r), tbb::auto_partitioner());
		}
	}
}

// factory
node_t *create_crop_node() { return new crop_node_t();}

const node_metaclass_t *crop_node_t::metaclass() const { return &crop_node_metaclass();}

const node_metaclass_t& crop_node_t::crop_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.crop";
        info.major_version = 1;
        info.minor_version = 0;
        info.menu = "Image";
        info.submenu = "Transform";
        info.menu_item = "Crop";
        info.create = &create_crop_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( crop_node_t::crop_node_metaclass());

} // namespace
} // namespace
