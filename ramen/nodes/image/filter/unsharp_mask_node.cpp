// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/nodes/image/filter/unsharp_mask_node.hpp>

#include<ramen/gil/extension/algorithm/tbb/tbb_transform2.hpp>

#include<ramen/params/float_param.hpp>
#include<ramen/params/popup_param.hpp>

#include<ramen/image/box_blur.hpp>

namespace ramen
{
namespace image
{
namespace
{

struct unsharp_mask_fun
{
    unsharp_mask_fun( float amount, float thereshold) : amount_(amount), thereshold_( thereshold) {}

    image::pixel_t operator()( const image::pixel_t& src, const image::pixel_t& blured) const
    {
		image::pixel_t result;

		for( int i = 0; i < 3; ++i)
		{
		    float diff = src[i] - blured[i];

		    if( abs( 2 * diff) < thereshold_)
				diff = 0;

		    result[i] = src[i] + ( amount_ * diff);
		}

		result[3] = src[3];
		return result;
    }

    inline float abs( float x) const
    {
		if( x < 0)
		    return -x;

		return x;
    }

    float amount_;
    float thereshold_;
};

} // unnamed

unsharp_mask_node_t::unsharp_mask_node_t() : areaop_node_t() { set_name( "unsharp");}

void unsharp_mask_node_t::do_create_params()
{
    std::auto_ptr<float_param_t> p( new float_param_t( "Radius"));
    p->set_id( "radius");
    p->set_default_value( 3.0f);
    p->set_range( 0, 20);
    p->set_step( 0.5);
    add_param( p);

    p.reset( new float_param_t( "Amount"));
    p->set_id( "amount");
    p->set_default_value( 0.5f);
    p->set_range( 0, 3);
    p->set_step( 0.1);
    add_param( p);

    p.reset( new float_param_t( "Thereshold"));
    p->set_id( "theresh");
    p->set_default_value( 0.0f);
    p->set_range( 0, 3);
    p->set_step( 0.1);
    add_param( p);
}

void unsharp_mask_node_t::get_expand_radius( int& hradius, int& vradius) const
{
    float radius = get_value<float>( param( "radius"));
    hradius = std::ceil( radius / aspect_ratio() * proxy_scale().x);
    vradius = std::ceil( radius * proxy_scale().y);
}

void unsharp_mask_node_t::do_process( const render::context_t& context)
{
    Imath::Box2i area( Imath::intersect( input_as<image_node_t>()->defined(), defined()));

    if( area.isEmpty())
		return;

    image::const_image_view_t src( input_as<image_node_t>()->const_subimage_view( area));
    image::image_view_t dst( subimage_view( area));

    float radiusx = get_value<float>( param( "radius")) / context.subsample * proxy_scale().x / 3.0f / aspect_ratio();
    float radiusy = get_value<float>( param( "radius")) / context.subsample * proxy_scale().y / 3.0f;

	image::buffer_t tmp( src.height(), src.width(), 4);
    image::box_blur_rgba( src, tmp.rgba_view(), dst, radiusx, radiusy, 3);
    boost::gil::tbb_transform2_pixels( src, dst, dst, unsharp_mask_fun( get_value<float>( param( "amount")),
																		 get_value<float>( param( "theresh"))));

    boost::gil::copy_pixels( boost::gil::nth_channel_view( input_as<image_node_t>()->const_subimage_view( area), 3),
							boost::gil::nth_channel_view( subimage_view( area), 3));
}

// factory
node_t *create_unsharp_mask_node() { return new unsharp_mask_node_t();}

const node_metaclass_t *unsharp_mask_node_t::metaclass() const { return &unsharp_mask_node_metaclass();}

const node_metaclass_t& unsharp_mask_node_t::unsharp_mask_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
		info.id = "image.builtin.unsharp_mask";
		info.major_version = 1;
		info.minor_version = 0;
		info.menu = "Image";
		info.submenu = "Filter";
		info.menu_item = "Unsharp Mask";
		info.create = &create_unsharp_mask_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( unsharp_mask_node_t::unsharp_mask_node_metaclass());

} // namespace
} // namespace
