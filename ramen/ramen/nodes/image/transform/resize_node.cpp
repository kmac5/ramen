// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/transform/resize_node.hpp>

#include<OpenEXR/ImathMath.h>

#include<ramen/app/composition.hpp>

#include<ramen/params/popup_param.hpp>
#include<ramen/params/combo_group_param.hpp>
#include<ramen/params/float2_param.hpp>
#include<ramen/params/image_format_param.hpp>

#include<ramen/image/resize.hpp>

namespace ramen
{
namespace image
{
namespace
{

enum
{
    lanczos3_filter = 0,
    mitchell_filter,
    catrom_filter
};

} // unnamed

resize_node_t::resize_node_t() : image_node_t()
{
    add_input_plug( "front", false, ui::palette_t::instance().color("front plug"), "Front");
    add_output_plug();
	set_name( "resize");
	scale_ = Imath::V2f( 0, 0);
}

resize_node_t::resize_node_t(const resize_node_t& other) : image_node_t(other), scale_( other.scale_) {}

void resize_node_t::do_create_params()
{
    std::auto_ptr<combo_group_param_t> top( new combo_group_param_t( "Method"));
    top->set_id( "method");

    // Absolute
    {
		std::auto_ptr<composite_param_t> group( new composite_param_t( "Size"));
		group->set_id( "size_group");

		std::auto_ptr<image_format_param_t> fp( new image_format_param_t( "Size"));
		fp->set_id( "size");
		group->add_param( fp);
		top->add_param( group);
    }

    // Relative
    {
		std::auto_ptr<composite_param_t> group( new composite_param_t( "Scale"));
		group->set_id( "scale_group");

		std::auto_ptr<float2_param_t> fp( new float2_param_t( "Scale"));
		fp->set_id( "scale");
		fp->set_min( 0);
		fp->set_step( 0.05);
		fp->set_default_value( Imath::V2f( 1, 1));
		fp->set_proportional( true);
		fp->set_static( true);
		group->add_param( fp);
        top->add_param( group);
    }

    add_param( top);
    std::auto_ptr<popup_param_t> p( new popup_param_t( "Filter"));
    p->set_id( "filter");
    p->menu_items() = boost::assign::list_of( "Lanczos3")( "Mitchell")( "Catrom");
    add_param( p);
}

bool resize_node_t::do_is_identity() const
{
	const image_node_t *in = input_as<const image_node_t>();
	Imath::V2f scale;

    if( get_value<int>( param( "method")) == 0)
    {
		image::format_t f( get_value<image::format_t>( param( "size")));
		Imath::Box2i box( f.area());
		scale.x = ( float) ( box.max.x - 1) / in->format().size().x;
		scale.y = ( float) ( box.max.y - 1) / in->format().size().y;
    }
    else
		scale = get_value<Imath::V2f>( param( "scale"));

	return scale.x == 1.0f && scale.y == 1.0f;
}

void resize_node_t::do_calc_format( const render::context_t& context)
{
	image_node_t *in = input_as<image_node_t>();

    if( get_value<int>( param( "method")) == 0)
    {
		image::format_t f( get_value<image::format_t>( param( "size")));
		Imath::Box2i box( f.area());
		scale_.x = ( float) ( box.max.x - 1) / in->format().size().x;
		scale_.y = ( float) ( box.max.y - 1) / in->format().size().y;
		set_aspect_ratio( f.aspect);
    }
    else
    {
		Imath::V2f scale = get_value<Imath::V2f>( param( "scale"));
		scale_.x = scale.x;
		scale_.y = scale.y;
		set_aspect_ratio( in->aspect_ratio());
    }

	set_format( image::resize_box( in->format(), in->format().min, 1.0f / scale_.x, 1.0f / scale_.y));
	set_proxy_scale( in->proxy_scale());
}

void resize_node_t::do_calc_bounds( const render::context_t& context) { set_bounds( format());}

void resize_node_t::do_calc_inputs_interest( const render::context_t& context)
{
	image_node_t *in = input_as<image_node_t>();
	Imath::Box2i roi( image::resize_box( interest(), in->format().min, scale_.x, scale_.y));

	int xfilter_area, yfilter_area;

    switch( get_value<int>( param( "filter")))
    {
		case lanczos3_filter:
		{
			lanczos3_filter_t filter;
			xfilter_area = filter.filter_area( scale_.x);
			yfilter_area = filter.filter_area( scale_.y);
		}
		break;

		case mitchell_filter:
		{
			mitchell_filter_t filter;
			xfilter_area = filter.filter_area( scale_.x);
			yfilter_area = filter.filter_area( scale_.y);
		}
		break;

		case catrom_filter:
		{
			catrom_filter_t filter;
			xfilter_area = filter.filter_area( scale_.x);
			yfilter_area = filter.filter_area( scale_.y);
		}
		break;
	};

	roi.min.x -= xfilter_area / 2 + 2;
	roi.min.y -= yfilter_area / 2 + 2;
	roi.max.x += xfilter_area / 2 + 1;
	roi.max.y += yfilter_area / 2 + 1;
	in->add_interest( roi);
}

void resize_node_t::do_process( const render::context_t& context)
{
	RAMEN_ASSERT( !( scale_.x == 1.0f && scale_.y == 1.0f));

	const image_node_t *in = input_as<const image_node_t>();

    switch( get_value<int>( param( "filter")))
    {
		case lanczos3_filter:
			image::resize_lanczos3( in->const_image_view(), in->defined(),
									image_view(), defined(), in->format().min, scale_);
		break;

		case mitchell_filter:
			image::resize_mitchell( in->const_image_view(), in->defined(),
									image_view(), defined(), in->format().min, scale_);
		break;

		case catrom_filter:
			image::resize_catrom( in->const_image_view(), in->defined(),
									image_view(), defined(), in->format().min, scale_);
		break;
    }
}

// factory
node_t *create_resize_node() { return new resize_node_t();}

const node_metaclass_t *resize_node_t::metaclass() const { return &resize_node_metaclass();}

const node_metaclass_t& resize_node_t::resize_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.resize";
        info.major_version = 1;
        info.minor_version = 0;
        info.menu = "Image";
        info.submenu = "Transform";
        info.menu_item = "Resize";
        info.create = &create_resize_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( resize_node_t::resize_node_metaclass());

} // namespace
} // namespace
